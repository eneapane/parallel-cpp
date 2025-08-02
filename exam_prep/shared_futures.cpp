#include <iostream>
#include <future>
#include <thread>
#include <chrono>
#include <utility>
#include <vector>
#include <unordered_map>
#include <string>
#include <functional>
#include <random>

// Simple cell value type
using CellValue = double;

// Cell reference type for dependencies
using CellRef = std::string;

// Forward declaration
class SpreadsheetCell;

// Spreadsheet class to manage cells and their dependencies
class ParallelSpreadsheet {
private:
    std::unordered_map<CellRef, std::shared_ptr<SpreadsheetCell>> cells;
    std::unordered_map<CellRef, std::shared_future<CellValue>> cell_futures;

public:
    // Add a cell with a formula that may depend on other cells
    void add_cell(const CellRef& ref, std::function<CellValue(const std::vector<CellValue>&)> formula,
                  const std::vector<CellRef>& dependencies = {});

    // Execute all cells in parallel
    void execute_parallel();

    // Get the result of a cell (blocks until ready)
    CellValue get_cell_value(const CellRef& ref);

    // Print all cell values
    void print_results();
};

// Individual spreadsheet cell
class SpreadsheetCell {
private:
    CellRef reference;
    std::function<CellValue(const std::vector<CellValue>&)> formula;
    std::vector<CellRef> dependencies;
    std::promise<CellValue> promise;
    std::shared_future<CellValue> future;
    ParallelSpreadsheet* spreadsheet;

public:
    SpreadsheetCell(CellRef  ref,
                    std::function<CellValue(const std::vector<CellValue>&)> f,
                    const std::vector<CellRef>& deps,
                    ParallelSpreadsheet* sheet)
        : reference(std::move(ref)), formula(std::move(f)), dependencies(deps), spreadsheet(sheet) {
        future = promise.get_future().share();
    }

    [[nodiscard]] std::shared_future<CellValue> get_future() const {
        return future;
    }

    // Execute this cell's formula in a separate thread
    void execute_async() {
        std::thread([this]() {
            try {
                // Simulate some computation time
                std::this_thread::sleep_for(std::chrono::milliseconds(100 + rand() % 300));

                std::cout << "Starting calculation for cell " << reference << std::endl;

                // Gather dependency values (this will block until dependencies are ready)
                std::vector<CellValue> dep_values;
                for (const auto& dep : dependencies) {
                    CellValue dep_value = spreadsheet->get_cell_value(dep);
                    dep_values.push_back(dep_value);
                }
                // Calculate result
                CellValue result = formula(dep_values);

                // Set the promise with the result
                promise.set_value(result);
            } catch (const std::exception& e) {
                std::cout << "Error in cell " << reference << ": " << e.what() << std::endl;
                promise.set_exception(std::current_exception());
            }
        }).detach();
    }
};

void ParallelSpreadsheet::add_cell(const CellRef& ref,
                                   std::function<CellValue(const std::vector<CellValue>&)> formula,
                                   const std::vector<CellRef>& dependencies) {
    auto cell = std::make_shared<SpreadsheetCell>(ref, formula, dependencies, this);
    cells[ref] = cell;
    cell_futures[ref] = cell->get_future();
}

void ParallelSpreadsheet::execute_parallel() {
    std::cout << "Starting parallel execution of " << cells.size() << " cells..." << std::endl;

    // Start all cells executing in parallel
    for (auto& [ref, cell] : cells) {
        cell->execute_async();
    }
}

CellValue ParallelSpreadsheet::get_cell_value(const CellRef& ref) {
    auto it = cell_futures.find(ref);
    if (it != cell_futures.end()) {
        return it->second.get(); // This blocks until the value is ready
    }
    throw std::runtime_error("Cell " + ref + " not found");
}

void ParallelSpreadsheet::print_results() {
    std::cout << "\n=== Final Results ===" << std::endl;
    for (const auto& [ref, future] : cell_futures) {
        try {
            CellValue value = future.get();
            std::cout << "Cell " << ref << " = " << value << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Cell " << ref << " = ERROR: " << e.what() << std::endl;
        }
    }
}

// Demo function to create a sample spreadsheet
void demo_parallel_spreadsheet() {
    ParallelSpreadsheet sheet;

    // Create a dependency graph that looks like:
    // A1 (constant) → B1 (A1 * 2) → D1 (B1 + C1)
    // A2 (constant) → B2 (A2 * 3) ↗
    // C1 (A1 + A2) ↗
    // E1 (D1 * 2)

    // Base cells (no dependencies)
    sheet.add_cell("A1", [](const std::vector<CellValue>&) -> CellValue {
        return 10.0;
    });

    sheet.add_cell("A2", [](const std::vector<CellValue>&) -> CellValue {
        return 5.0;
    });

    // Cells with single dependencies
    sheet.add_cell("B1", [](const std::vector<CellValue>& deps) -> CellValue {
        return deps[0] * 2.0; // A1 * 2
    }, {"A1"});

    sheet.add_cell("B2", [](const std::vector<CellValue>& deps) -> CellValue {
        return deps[0] * 3.0; // A2 * 3
    }, {"A2"});

    // Cell with multiple dependencies
    sheet.add_cell("C1", [](const std::vector<CellValue>& deps) -> CellValue {
        return deps[0] + deps[1]; // A1 + A2
    }, {"A1", "A2"});

    // Cell depending on other computed cells
    sheet.add_cell("D1", [](const std::vector<CellValue>& deps) -> CellValue {
        return deps[0] + deps[1]; // B1 + C1
    }, {"B1", "C1"});

    // Final cell in the chain
    sheet.add_cell("E1", [](const std::vector<CellValue>& deps) -> CellValue {
        return deps[0] * 2.0; // D1 * 2
    }, {"D1"});

    // Execute all cells in parallel
    auto start_time = std::chrono::high_resolution_clock::now();
    sheet.execute_parallel();

    // Print results (this will block until all calculations are complete)
    sheet.print_results();

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "\nTotal execution time: " << duration.count() << " ms" << std::endl;
    std::cout << "\nNote: Cells executed in parallel based on dependencies." << std::endl;
    std::cout << "A1 and A2 ran immediately, B1/B2/C1 ran after A1/A2 completed," << std::endl;
    std::cout << "D1 ran after B1 and C1 completed, and E1 ran after D1 completed." << std::endl;
}
