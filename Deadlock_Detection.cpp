
#include <iostream>
#include <vector>
using namespace std;

bool dfs(int node, vector<vector<int>> &graph,
         vector<bool> &visited,
         vector<bool> &recStack,
         vector<bool> &deadlocked)
{
    visited[node] = true;
    recStack[node] = true;

    for (int next : graph[node])
    {
        if (!visited[next])
        {
            if (dfs(next, graph, visited, recStack, deadlocked))
            {
                deadlocked[node] = true;
                return true;
            }
        }
        else if (recStack[next])
        {
            deadlocked[node] = true;
            return true;
        }
    }

    recStack[node] = false;
    return false;
}

int main()
{
    int n, m;
    cout << "Enter number of processes: ";
    cin >> n;

    cout << "Enter number of resources: ";
    cin >> m;

    vector<vector<int>> allocation(n, vector<int>(m));
    vector<vector<int>> request(n, vector<int>(m));

    cout << "Enter Allocation Matrix:\n";
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            cin >> allocation[i][j];

    cout << "Enter Request Matrix:\n";
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            cin >> request[i][j];

    // Build Wait-For Graph
    vector<vector<int>> graph(n);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (i != j)
            {
                for (int r = 0; r < m; r++)
                {
                    if (request[i][r] > 0 && allocation[j][r] > 0)
                    {
                        graph[i].push_back(j);
                        break;
                    }
                }
            }
        }
    }

    vector<bool> visited(n, false);
    vector<bool> recStack(n, false);
    vector<bool> deadlocked(n, false);

    for (int i = 0; i < n; i++)
    {
        if (!visited[i])
            dfs(i, graph, visited, recStack, deadlocked);
    }

    bool found = false;
    for (int i = 0; i < n; i++)
    {
        if (deadlocked[i])
        {
            if (!found)
            {
                cout << "Deadlock detected in processes: ";
                found = true;
            }
            cout << "P" << i << " ";
        }
    }

    if (!found)
        cout << "No Deadlock detected";

    return 0;
}
