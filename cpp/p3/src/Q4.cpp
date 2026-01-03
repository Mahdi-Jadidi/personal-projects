#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
using namespace std;

int n, m;
vector<string> grid;
vector<vector<vector<bool>>> visited; 
const int dx[] = {-1, 1, 0, 0};
const int dy[] = {0, 0, -1, 1};
bool isValid(int x, int y, int mode, char empire) {
    if (x < 0 || x >= n || y < 0 || y >= m || visited[x][y][mode])
        return false;
    if (mode == 0) return grid[x][y] == empire;       
    if (mode == 1) return grid[x][y] == '~' || grid[x][y] == '%'; 
    return false;
}

int dfs(int x, int y, char empire, int mode) {
    visited[x][y][mode] = true;
    int count = (mode == 0 ? 1 : 0); 
    for (int i = 0; i < 4; i++) {
        int nx = x + dx[i], ny = y + dy[i];
        if (mode == 0) { 
            if (isValid(nx, ny, 0, empire)) {
                count += dfs(nx, ny, empire, 0); 
            } else if (isValid(nx, ny, 1, empire)) {
                count += dfs(nx, ny, empire, 1); 
            }
        } else {
            if (isValid(nx, ny, 1, empire)) {
                count += dfs(nx, ny, empire, 1); 
            } else if (grid[x][y] == '%' && isValid(nx, ny, 0, empire)) {
                count += dfs(nx, ny, empire, 0); 
            }
        }
    }
    return count;
}

int main() {
    cin >> m >> n;
    grid.resize(n);
    for (int i = 0; i < n; i++) {
        cin >> grid[i];
    }
    unordered_map<char, int> max_territory;
    visited.assign(n, vector<vector<bool>>(m, vector<bool>(2, false)));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (grid[i][j] >= '0' && grid[i][j] <= '9' && !visited[i][j][0]) {
                char empire = grid[i][j];
                int size = dfs(i, j, empire, 0);
                max_territory[empire] = max(max_territory[empire], size);
            }
        }
    }
    for (char c = '0'; c <= '9'; c++) {
        cout << (max_territory.find(c) != max_territory.end() ? max_territory[c] : 0) << endl;
    }
    return 0;
}
