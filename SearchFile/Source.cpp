#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>
#include <filesystem>

using namespace std;

mutex printMutex;
atomic<bool> stopSearch(false);

void searchFile(const filesystem::path& dir, const string& fileName, int maxThreads)
{
    if (stopSearch)
    {
        return;
    }

    for (const auto& entry : filesystem::directory_iterator(dir))
    {
        if (stopSearch)
        {
            return;
        }

        if (entry.is_directory())
        {
            if (maxThreads > 0)
            {
                thread(searchFile, entry.path(), fileName, maxThreads - 1).detach();
            }
        }
        else if (entry.is_regular_file() && entry.path().filename() == fileName)
        {
            lock_guard<mutex> lock(printMutex);
            cout << "Found file: " << entry.path() << endl;
            stopSearch = true;
            return;
        }
    }
}

int main() {
    string fileName = "name"; // Replace with your target file name
    filesystem::path rootDir = "C:\\";

    int maxThreads = min(8, static_cast<int> (thread::hardware_concurrency())); // Limit threads to 8

    searchFile(rootDir, fileName, maxThreads);

    this_thread::sleep_for(chrono::seconds(1)); // Wait for threads to finish

    return 0;
}
