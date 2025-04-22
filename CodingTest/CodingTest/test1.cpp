#include <iostream>
#include <vector>
std::vector<std::vector<int>> Miro;
std::vector<int> CanFindTimes;
int Row,Col;
void FindMiro(int CurrentCount, int X, int Y, std::vector<std::vector<int>> CurrentPath){
    CurrentPath[X][Y] = 1;

    ++CurrentCount;

    if(X == Col - 1 && Y == Row - 1){
        CanFindTimes.emplace_back(CurrentCount);
        return;
    }

    // 오른쪽이 끝이 아니며, 
    // 오른쪽이 길이며,
    // 오른쪽을 간적이 없다면
    if(X != Col -1 && 
        Miro[Y][X+1] == 1 &&
        CurrentPath[Y][X+1] != 1)
    {
        FindMiro(CurrentCount +1, X+1, Y, CurrentPath);
    }

    // 왼쪽이 끝이 아니며, 
    // 왼쪽이 길이며,
    // 왼쪽을 간적이 없다면
    if(X != 0 && 
        Miro[Y][X-1] == 1 &&
        CurrentPath[Y][X-1] != 1)
    {
        FindMiro(CurrentCount +1, X-1, Y, CurrentPath);
    }

    // 아래쪽이 끝이 아니며, 
    // 아래쪽이 길이며,
    // 아래쪽을 간적이 없다면
    if(Y != Row-1 && 
        Miro[Y+1][X] == 1 &&
        CurrentPath[Y+1][X] != 1)
    {
        FindMiro(CurrentCount +1, X, Y+1, CurrentPath);
    }

    // 위쪽 끝이 아니며, 
    // 위쪽이 길이며,
    // 위쪽을 간적이 없다면
    if(Y != 0 && 
        Miro[Y-1][X] == 1 &&
        CurrentPath[Y-1][X] != 1)
    {
        FindMiro(CurrentCount +1, X, Y-1, CurrentPath);
    }

}

int main(){

    std::cin >> Row >> Col; 
    Miro.resize(Row);
    for(int i = 0; i < Row; ++i){
        Miro[i].reserve(Col);
        for(int j = 0; j < Col; ++j){
            char Kan;
            std::cin >> Kan;
        	Miro[i].emplace_back(Kan - '0');
        }
    }
    std::vector<std::vector<int>> CurrentPath;
    CurrentPath.resize(Row);
    for(int i = 0; i < Row; ++i){
        CurrentPath[i].reserve(Col);
        for(int j = 0; j < Col; ++j){
            CurrentPath[i].emplace_back(0);
        }
    }
    FindMiro(0,0,0, CurrentPath);

    int LessPath = 99999999;
    for(int PathCount : CanFindTimes){
        if(PathCount < LessPath){
            LessPath = PathCount;
        }
    }

    std::cout << LessPath << std::endl;
}