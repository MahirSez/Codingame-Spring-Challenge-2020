#include<bits/stdc++.h>
#define pii pair<int,int>
#define ppp pair<pii , pii>
#define uu first
#define vv second
using namespace std;

const int INF = 1e9;
const int N = 10;
const int MAX = 40;
const double LEAF_REVEAL = 55.00;


map<string, string>kills;

int dx[] = { 0 , 0 , 1 , -1};
int dy[] = {1 , -1 , 0 , 0};

int n , m;
vector< vector<int> > original , board , degree , leafMark;

int nowX[N] , nowY[N];
int jmpX[N] , jmpY[N];
int tgtX[N] , tgtY[N];
int nxtX[N] , nxtY[N];

int myLastX[N][N] , myLastY[N][N];

int enemNowX[N] , enemNowY[N];
int enemLastX[N] , enemLastY[N];

int coolDownRem[N];

int isAliveMy[N] , myPacBusy[N];
int enemVisible[N] , enemPacBusy[N];
int enemDead[N];

int cellTaken[MAX][MAX];
int gridDivisionId[MAX][MAX];

int mySpeedLeft[N] , enemSpeedLeft[N];

string switchTo[N] , pacTypeAt[MAX][MAX]  , myPacType[N];

int bigPalletTaken;
int totMovesDone;
int totLeaf ;
int leafRevealed;

double gamePercentage;

void print(vector< vector<int> > vec) {    ///OK
    for(auto x : vec) {
        for(auto xx : x) {
            cerr<<xx<<"\t";
        }
        cerr<<endl;
    }
    cerr<<endl;
}

void detectLeaves() {       ///NEW
    
    stack<pii>st;
    
    for(int i =0 ; i < n ; i++ ) {
        for(int j =0 ; j < m ; j++ ) {
            
            if(original[i][j] == -1) continue;
            
            for(int k = 0; k < 4 ; k++ ) {
                
                int xx = (i + dx[k] + n )%n;
                int yy = (j + dy[k] + m )%m;
                if(original[xx][yy] == 0) degree[i][j]++;
            }
            if(degree[i][j] == 1) st.push({i,j});
        }
    }
    while(!st.empty()) {
        
        int tx = st.top().uu;
        int ty = st.top().vv;
        st.pop();
//        cerr<<"baad: "<<ty<<" "<<tx<<endl;
        leafMark[tx][ty] = 1;
        totLeaf++;
        
        for(int k = 0; k < 4 ; k++ ) {
            
            int xx = (tx + dx[k] + n )%n;
            int yy = (ty + dy[k] + m )%m;
            if(original[xx][yy] == -1) continue;
            degree[xx][yy]--;
            if(degree[xx][yy] == 1) st.push({xx,yy});
        }
    }
//    print(leafMark);
    cerr<<totLeaf<<endl;
}

void boardInput() {     ///OK

    cin >> m >> n; cin.ignore();
    for (int i = 0; i < n; i++) {
        string row;
        getline(cin, row);
        
        vector<int>tmp;
        for(auto x : row) {
            if(x == ' ') tmp.push_back(0);
            else tmp.push_back(-1);
        }
        original.push_back(tmp);
    }
    
    board = original;
    degree = original;
    leafMark = original;
    
    detectLeaves();
    
    for(int i =0 ; i < n ; i++) {
        for(int j =0 ;j < m ; j++ ) {
            if(board[i][j] == 0 && leafMark[i][j] == 0) board[i][j] = 1;
        }
    }
    
    
}

int getCompSizeAt(int x , int y) {      ///OK
    vector< vector<int> > dist = board;
    queue<pii>q;
    q.push({x,y});
    dist[x][y] = 1000;
    
    int ret = 1;
    while(!q.empty()) {
        
        int tx = q.front().uu;
        int ty = q.front().vv;
        q.pop();
        for(int k =0 ; k < 4; k++ ) {
            int xx = (tx + dx[k] + n )%n;
            int yy = (ty + dy[k] + m )%m;
            
            if(dist[xx][yy] == 1) {
                ret++;
                dist[xx][yy] = dist[tx][ty] + 1;
                q.push({xx,yy});
            }
        }
    }
    return ret;
}

int manhattanDistance(int x1 , int y1 , int x2 , int y2) {      ///OK
    int xx = abs(x1 - x2);
    xx = min(xx , n - xx);
    int yy = abs(y1 - y2);
    yy = min(yy , m - yy);
    return xx + yy;
}

int distanceBetween(int x1 , int y1 , int x2 , int y2) {        ///OK
    
//    cerr<<y1 <<" "<<x1<<" "<<y2<<" "<<x2<<endl;
    vector<  vector<int> > dist = original;
    queue<pii>q;
    q.push({x1 , y1});
    dist[x1][y1] = 1000;
    
    while(!q.empty()) {
        int tx = q.front().uu;
        int ty = q.front().vv;
        q.pop();
        for(int k =0 ; k < 4; k++ ) {
            int xx = (tx + dx[k] + n )%n;
            int yy = (ty + dy[k] + m )%m;
            int newDist = dist[tx][ty] + 1;
            if(xx == x2 && yy == y2) return newDist - dist[x1][y1];
            if(dist[xx][yy] == 0) {
                dist[xx][yy] = newDist;
                q.push({xx,yy});
            }
        }
    }
    cerr<<"errrrrrrrrrrrrr distance unreachable  "<<y1<<" "<<x1<<" = "<<original[x1][y1]<<" , "<<y2<<" "<<x2<<" = "<<original[x2][y2]<<endl;
    return dist[x2][y2] - dist[x1][y1];
}

void stayStill() {      /// OK
    
    for(int i =0 ; i < N ; i++ ) {
        
        if(!isAliveMy[i] || myPacBusy[i]) continue;
        cerr<<" Staying stillll "<<i<<endl;
        jmpX[i] = nxtX[i] = nowX[i];
        jmpY[i] = nxtY[i] = nowY[i];
        
    }
}

void resolveClash() {       /// OK But Will Cause problem if nxtX or jmpX is assigned abruptly
    
    for(int i =0 ; i < N ; i++ ) {
            
        if(!isAliveMy[i]) continue;
        for(int j =i +1 ; j < N ; j++  ) {
                
            if(!isAliveMy[j]) continue;
            if(nxtX[i] == nxtX[j] && nxtY[i] == nxtY[j]) {
                
                cerr<<"STOP "<<j<<endl;
                jmpX[j] = nowX[j];
                jmpY[j] = nowY[j];
                
                nxtX[j] = nowX[j];
                nxtY[j] = nowY[j];
                break;
            }
        }
    }
}

void printMove() {      ///OK

    for(int i =0 ; i < N ; i++ ) {
        
        if(!isAliveMy[i]) continue;        
        cerr<<i<<" --jmp: "<<jmpY[i]<<" "<<jmpX[i]<<" --tgt: "<<tgtY[i]<<" "<<tgtX[i]<<endl;
        if(switchTo[i] == "" || switchTo[i] == "none") cout<<"MOVE "<<i<<" "<<jmpY[i]<<" "<<jmpX[i]<<" id: "<<i<<" | ";
        else if(switchTo[i] == "SPEED") cout<<switchTo[i]<<" "<<i<<" | ";
        else cout<<"SWITCH "<<i<<" "<<switchTo[i]<<" | ";
    }
    cout<<endl;
}

void print() {      ///OK

    for(int i = 0; i < n ; i++ ) {
        
        for(int j = 0; j < m ; j++ ) {
            
            if(board[i][j] == -1) cerr<<".";
            else if(board[i][j] == 10) cerr<<"9";
            else if(board[i][j] == -5) cerr<<"-";
            else if(board[i][j] == 5) cerr<<"+";
            else cerr<<board[i][j];
        }
        cerr<<endl;
    }
    cerr<<endl;
}

void updateTwo() {      ///OK
    
    for(int i =0 ; i < N ; i++ ) {
        if(!isAliveMy[i]) continue;
//        cerr<<"ok " <<i<<" "<<nxtX[i]<<" "<<nxtY[i]<<" "<<jmpX[i]<<" "<<jmpY[i]<<endl;
        if(original[nowX[i]][nowY[i]] != -1)  board[nowX[i]][nowY[i]] = 2;
        
        if( switchTo[i] == "") {            
            if(original[nxtX[i]][nxtY[i]] != -1 ) board[nxtX[i]][nxtY[i]] = 2;
            if(original[jmpX[i]][jmpY[i]] != -1 ) board[jmpX[i]][jmpY[i]] = 2;
        }
    }
    
}

void updateSpeed() {        ///OK
    for(int i =0 ; i < N ; i++ ) {
        if(!isAliveMy[i]) continue;
        if(coolDownRem[i] == 0 && switchTo[i] == "") switchTo[i] = "SPEED";
    }
}

void myMemoryUpdateBeforeTakingPallet() {       ///OK
    
    for(int i =0 ; i < N ; i++ ) {
        if(!isAliveMy[i]) continue;
        
        for(int k =0 ; k < 4; k++ ) {
            int x = nowX[i];
            int y = nowY[i];
            int cnt= 0;
            
            while(board[x][y] != -1 && cnt < MAX) {
                if(board[x][y] == 1) board[x][y] = 0;
                x = ( x + dx[k] + n) % n;
                y = ( y + dy[k] + m) % m;
                cnt++;
            }
        }
    }
}

string checkEnemyFor(int id ) {     ///Updated

    for(int i =0 ; i < N ; i++ ) {
        
        if(!enemVisible[i]) continue;
        
        int dst = distanceBetween(nowX[id] , nowY[id] , enemNowX[i] , enemNowY[i] );
        
        if(enemSpeedLeft[i] == 0 && dst <=1 ) return pacTypeAt[ enemNowX[i] ][ enemNowY[i] ];       ///New Update
        else if(enemSpeedLeft[i]> 0 && dst <=2 ) return pacTypeAt[ enemNowX[i] ][ enemNowY[i] ];
    }
    return "";
}

void updateMode() {     ///TESTING
    
    if(totMovesDone == 1) return;
    
    for(int i =0 ; i < N ; i++ ) {  
        if(!isAliveMy[i]) continue;
        if(coolDownRem[i] > 0) continue; 
               
        bool wasSame = 1;
        for(int j = 1 ; j <= 2 ;  j++ ) {
            
            if(myLastX[i][j] != nowX[i] || myLastY[i][j] != nowY[i] )  {
                wasSame = 0;
            }
        }
        if(wasSame) {       ///Was same for last 2 moves, meaning clash was going on
//            switchTo[i] = kills[ pacTypeAt[ nowX[i] ][ nowY[i] ] ];     
            switchTo[i] = "none";
            continue;
        }
           
        string enemyMode = checkEnemyFor(i);
        if(enemyMode == "") continue;

        if( pacTypeAt[ nowX[i] ][ nowY[i] ] == kills[enemyMode]) continue;      ///Already in the killing mode
        switchTo[i] = kills[enemyMode];
        cerr<<"Changing pac: "<<i<<" to: "<<kills[enemyMode]<<endl;
    }
}

void clearBoardForEnemAt(int x , int y ) {     ///OK
    
    for(int k =0 ; k < 4 ; k++ ) {
            
        int xx = x;
        int yy = y;
        int cnt= 0;
        bool removeOk = 1;
        while(board[xx][yy] != -1 && cnt < MAX) {
            
            if(board[xx][yy] == 5  ) removeOk = 0;
            xx = ( xx + dx[k] + n) % n;
            yy = ( yy + dy[k] + m) % m;
            cnt++;
        }
        if(!removeOk) continue;
        
        cnt = 0;
        xx = x;
        yy = y;
        cerr<<" Clearing at "<<yy<<" "<<xx<<endl;
        while(board[xx][yy] != -1 && cnt < MAX) {
            if(board[xx][yy] == 1) board[xx][yy] = 0;
            
            xx = ( xx + dx[k] + n) % n;
            yy = ( yy + dy[k] + m) % m;
            cnt++;
        }
    }
}

bool myBotInOneDIst(int x , int y) {        ///NEW

    for(int k =0 ; k < 4 ; k++ ) {
        int xx = (x + dx[k] + n) %n;
        int yy = (y + dy[k] + m) %m;
        if(board[xx][yy] == 5) return 1;
    }
    return 0;
}

vector<pii> opponentBetterPath(vector<pii>&v1 , vector<pii>&v2) {      
    
    for(auto x : v1) if(board[x.uu][x.vv] == 5) return v2;
    for(auto x : v2) if(board[x.uu][x.vv] == 5) return v1;
    
    bool cancelPath1 = myBotInOneDIst(v1.back().uu , v1.back().vv );       ///NEW OK
    bool cancelPath2 = myBotInOneDIst(v2.back().uu , v2.back().vv );
    
    if(cancelPath1) return v2;
    if(cancelPath2) return v1;
    
    int pt1 = 0 , pt2 = 0;
    for(int i = 1 ;i < v1.size() ; i++ ) {
        pii x = v1[i];
        if( board[x.uu][x.vv] == -5) break;
        if( board[x.uu][x.vv] == 1) pt1 ++;
        if( board[x.uu][x.vv] == 10) pt1 += 10;
    }
    for(int i = 1 ; i < v2.size() ; i++ ) {
        pii x = v2[i];
        if( board[x.uu][x.vv] == -5) break;
        if( board[x.uu][x.vv] == 1) pt2 ++;
        if( board[x.uu][x.vv] == 10) pt2 += 10;
    }
    
    if(pt1 > pt2) return v1;
    return v2;
}

void print(vector<pii>&vec) {
    for(auto x : vec) {
        cerr<<x.vv<<" "<<x.uu<<" ,";
    }
    cerr<<endl;
}

vector<pii> opponentPredictionBFS(int x , int y) {      ///OK
    
    vector< vector<int> > dist = board;
    vector<pii>ret;
    ret.push_back({x,y});
    queue< vector<pii> > q;
    
    dist[x][y] = 1000;
    q.push(ret);
    while(!q.empty()) {
        
        vector<pii>top = q.front();
        q.pop();
        int tx = top.back().uu;
        int ty = top.back().vv;
        
        ret = opponentBetterPath(ret , top);
        
        if(dist[tx][ty] == 1005 || board[tx][ty] == 10 ) continue;
        
        for(int k =0 ; k < 4 ; k++ ) {
            
            int xx = (tx + dx[k] + n) %n;
            int yy = (ty + dy[k] + m) %m;
            
            if(dist[xx][yy] == -1 || dist[xx][yy] >= 1000) continue;
            dist[xx][yy] = dist[tx][ty] + 1;
            
            vector<pii>tmp = top;
            tmp.push_back({xx,yy});
            q.push(tmp);
        }
    }
    
    return ret;
}

void opponentMemoryUpdate() {       ///OK
    
    if(totMovesDone <= 2) return;
    for(int i =0 ; i < N ; i++ ) {
        if(!enemVisible[i]) continue;
        
        if(enemNowX[i] == enemLastX[i] && enemNowY[i] == enemLastY[i]) {
            cerr<<" pac: "<<i<<" I'll skip predicting you"<<endl;
            continue;
        }

        cerr<<" pac :"<<i<<" , I'll predict u"<<endl;
        
        cerr<<enemNowX[i]<<" "<<enemNowY[i]<<endl;
        vector<pii>vec = opponentPredictionBFS(enemNowX[i] , enemNowY[i]);
        
        for(auto x : vec) {
            if(board[x.uu][x.vv] == 1 ) {
                cerr<<"Clearing for opponent: "<<i<<" "<<x.vv<<" "<<x.uu<<endl;
                board[x.uu][x.vv] = 0;
            }
        }
    }
    print();
}

void deadPacUpdate(int pacId , int mine , int x , int y) {      ///OK

    if(!mine) {
        enemDead[pacId] = 1;
        return;
    }
    if(isAliveMy[pacId]) {
        isAliveMy[pacId] = 0;
        
        cerr<<"updating for my dead: "<<pacId<<endl;
        board[x][y] = -5;
        
        pacTypeAt[x][y] = kills[ myPacType[pacId] ];
        
        myPacType[pacId] = "DEAD";
        
        clearBoardForEnemAt(x , y);
    }
    cerr<<"DEAD "<<pacId<<" "<<mine<<" -- "<<y<<" "<<x<<endl;
}

struct simplePalletInfo {       ///NEW
    int  pacId , dist , palletX , palletY , compSize;
    simplePalletInfo(int a , int b , int c , int d ,int e ) : pacId(a) , dist(b) , palletX(c) , palletY(d) , compSize(e) {}
};

bool simplePalletComp(simplePalletInfo &n1 , simplePalletInfo &n2) {      ///NEW
    
    if( abs(n1.dist - n2.dist) == 0 ) return n1.compSize > n2.compSize;
    return n1.dist < n2.dist;
}

ppp getPalletToPac(int cellX , int cellY , int pacX , int pacY, int point ) {         ///NEW   returns nxtXY , jmpXY (from cellXY to pacXY)
    
    int retNxtX = -1 , retNxtY = -1, retJmpX = -1, retJmpY = -1;
    
    vector< vector<int> > dist = board;
    queue<ppp>q;
    q.push({ {cellX , cellY} , {cellX , cellY} });
    dist[cellX][cellY] = 1000;
    
    bool ok = 0;
        
    while(!ok && !q.empty()) {
            
        int prevX = q.front().uu.uu;
        int prevY = q.front().uu.vv;
            
        int tx = q.front().vv.uu;
        int ty = q.front().vv.vv;
        q.pop();
        
        for(int k =0 ; k < 4 ; k++ ) {
            
            int xx = (tx + dx[k] + n )%n;
            int yy = (ty + dy[k] + m )%m;
            
            int newDist = dist[tx][ty] + 1;
            
            if(dist[xx][yy] == -1) continue;
            if(dist[xx][yy] >= 1000) continue;
            
            if(dist[xx][yy] == -5 && ( kills[ pacTypeAt[ pacX ][ pacY ] ] == pacTypeAt[xx][yy] )  && distanceBetween(pacX , pacY , xx,yy) <=3 )  {    
                continue; 
            }

            if(xx == pacX && yy == pacY) {
                
                retNxtX = tx;
                retNxtY = ty;
                
                if(tx == prevX && ty == prevY ) {    
                    
                    int boardVal = board[tx][ty];
                    board[tx][ty] = 0;
                    
                    int mxCompVal = -1 ;
                    for(int kk =0 ; kk < 4 ; kk++ ) {
                        
                        int xxx = ( tx + dx[kk] + n ) % n;
                        int yyy = ( ty + dy[kk] + m ) % m;
                        
                        if(dist[xxx][yyy] == -1  ) continue;
                        if(xxx == xx && yyy == yy) continue;
                        int tmpCompVal = getCompSizeAt(xxx , yyy);
                        
                        if(tmpCompVal > mxCompVal) {
                            mxCompVal = tmpCompVal;
                            
                            retJmpX = xxx;
                            retJmpY = yyy;
                        }
                    }
                    board[tx][ty] = boardVal;
                    
                    if(mxCompVal == -1 ) {    ///otherwise, for 10 pts he might get misled giving oppoent the opportunity
                            
                        retJmpX = retNxtX;
                        retJmpY = retNxtY;
                    }
                }
                else {
                    retJmpX = prevX;
                    retJmpY = prevY;
                }
                
                ok = 1;
                break;
            }
            dist[xx][yy] = newDist;
            q.push({ {tx,ty} , {xx,yy}});
        }
    }
    return {{ retNxtX , retNxtY} , {retJmpX , retJmpY} };
    
}

struct superPalletInfo{         ///NEW
    int pacId , mine , dist , palletX , palletY;
    superPalletInfo(int a , int b , int c , int d , int e) : pacId(a) , mine(b) , dist(c) , palletX(d) , palletY(e) {}
};

bool superPalletComp(superPalletInfo &n1 , superPalletInfo &n2) {         ///NEW
    
    if(n1.dist == n2.dist) {
        
        int x1 = nowX[n1.pacId];
        int y1 = nowY[n1.pacId];
        
        int x2 = nowX[n2.pacId];
        int y2 = nowY[n2.pacId];
        
        if(n1.mine==1 && n2.mine == 0 && kills[  pacTypeAt[x1][y1] ] == pacTypeAt[x2][y2] ) return 0; ///New Condition
        return n1.mine > n2.mine;
    }
    return n1.dist < n2.dist;
}

void boardSearch10() {         ///TESTING
    
    vector<superPalletInfo>vec;
    
    for(int i =0 ; i < n ; i++ ) {
        for(int j =0 ; j < m ; j++ ) {
                
            if(board[i][j] != 10 ) continue;
            
            for(int ii = 0 ; ii < N ; ii++ ) {
                
                if(!isAliveMy[ii]) continue;
                
                int x = nowX[ii];
                int y = nowY[ii];
                int d = distanceBetween(x , y  , i , j);
                vec.push_back(superPalletInfo(ii , 1 , d , i , j));
                
                int xx = x;
                int yy = m - y - 1;
                
                enemNowX[ii] = xx;
                enemNowY[ii] = yy;
                if(pacTypeAt[xx][yy] == "") pacTypeAt[xx][yy] = pacTypeAt[x][y];
            }
        }
    }
    
    sort(vec.begin() , vec.end() , superPalletComp);
    int printCnt = 10;
    for(auto sp : vec) {
        
        int x = sp.palletX;
        int y = sp.palletY;
        int id = sp.pacId;
        
        if(printCnt) {
            cerr<<"pt: "<<10<<" -- dist: "<<sp.dist<<" ----- id: "<<id<<" -- tgt->("<<y<<" "<<x<<") --> pacbusy->"<<myPacBusy[id]<<" , celltaken->"<<cellTaken[x][y]<<endl;
            printCnt--;
        }
        if(cellTaken[x][y]) continue;
        
        if(myPacBusy[id]) {      
                
            int d13 = distanceBetween(nowX[id] , nowY[id] , x , y);
            int d12 = distanceBetween(nowX[id] , nowY[id] , tgtX[id] , tgtY[id]);
            int d23 = distanceBetween(tgtX[id] , tgtY[id] , x , y);
            if(d13 == d12 + d23) cellTaken[x][y] = 1;      
            continue;
        }
        
        tgtX[id] = x;
        tgtY[id] = y;
        
        ppp ret = getPalletToPac(x , y , nowX[id] , nowY[id] , 10);
        
        if(ret.uu.uu == -1) continue;       
        
        nxtX[id] = ret.uu.uu;
        nxtY[id] = ret.uu.vv;
        
        jmpX[id] = ret.vv.uu;
        jmpY[id] = ret.vv.vv;
        
        bool ok = 1;    
        
        for(int i =0 ; i < N ; i++) {   ///condition if another enemy is nearby on the path, NEEDED
            
            if(  kills[ pacTypeAt[ nowX[id] ][ nowY[id] ] ] == pacTypeAt[ enemNowX[i] ][ enemNowY[i] ] ) {
                
                int d13 = distanceBetween(nowX[id] , nowY[id] , enemNowX[i] , enemNowY[i]);
                 
                if(d13 >= 5 ) continue;      ///Don't change this value
                
                int d12 = distanceBetween(nowX[id] , nowY[id]  , nxtX[id] , nxtY[id]);
                int d23 = distanceBetween(enemNowX[i] , enemNowY[i] , nxtX[id] , nxtY[id]);
                
                if(d13 != d12 + d23) continue;
                cerr<<" cancelling "<<id<<" to "<<jmpY[id]<<" "<<jmpX[id]<<" , attacker = "<<i <<endl;
                ok = 0;
            }
        }
        if(ok) {
            cerr<<"Assigning 10: "<<id<<" to "<<y<<" "<<x<<endl;
            myPacBusy[id] = 2;
            cellTaken[x][y] = 1;
            
            cerr<<"Assigning 10: (enemy) "<<id<<" to "<<m-y-1<<" "<<x<<endl;            ///ok
            enemPacBusy[id] = 2;    
            cellTaken[x][m-y-1] = 1;
            
            if(enemVisible[id] ==0) {
                enemVisible[id] = 1;
                board[ nowX[id] ][ m - nowY[id] - 1 ] = -5;
                board[ nxtX[id] ][ m - nxtY[id] - 1 ] = 0;      ///new
                pacTypeAt[ nxtX[id] ][ m - nxtY[id] - 1 ] = pacTypeAt[ nowX[id] ][ nowY[id] ];      ///updated
            }
            
        }
    }
}

vector<simplePalletInfo> searchSinglePallets(int point) {      ///NEW stuffs added  

    vector<simplePalletInfo>vec;
    
    int pacSearchSpaceLimit = 100;      ///NEW
    int nowAliveMy = 0;
    for(int i =0 ; i < N ; i++ ) {
        if(!isAliveMy[i]) continue;
        nowAliveMy++;
    }
    pacSearchSpaceLimit = (300/max(nowAliveMy , 1));
    cerr<<"Search Space: "<<pacSearchSpaceLimit<<endl;      ///TEST
    
    for(int i =0 ; i < N ; i++ ) {
        
        if(!isAliveMy[i] || myPacBusy[i] ) continue;
        
        vector< vector<int> > dist = board;
        
        queue<pii>q;
        q.push({nowX[i] , nowY[i]});
        dist[nowX[i]][nowY[i]] = 1000;
        int cntTakenPallet = 0;
        
        while(!q.empty()) {
            
            int tx = q.front().uu;
            int ty = q.front().vv;
            q.pop();
            
            for(int k =0 ; k < 4 ; k++ ) {
                
                int xx = (tx + dx[k] + n )%n;
                int yy = (ty + dy[k] + m )%m;
                int newDist = dist[tx][ty] + 1;
                if(dist[xx][yy] == -1) continue;
                
                    
                if(dist[xx][yy] == -5 && ( kills[ pacTypeAt[ nowX[i] ][ nowY[i] ] ] == pacTypeAt[xx][yy] )  && distanceBetween(nowX[i] , nowY[i] , xx,yy) <=3 )  {    
                    cerr<<i<<" panik! run"<<endl;
                    continue; 
                }
                
                if(dist[xx][yy] >= 1000) continue;
                
                if(dist[xx][yy] == point) {
                    vec.push_back(simplePalletInfo( i , newDist , xx , yy , 0 ));
                    cntTakenPallet++;
                }
                dist[xx][yy] = newDist;
                q.push({xx,yy});
            }
            if(cntTakenPallet > pacSearchSpaceLimit) break;
        }
        
//        cerr<<i<<" took "<<cntTakenPallet<<endl;        
    }
    return vec;
    
}

void boardSearch(int point) {       ///NEW
    
    vector<simplePalletInfo>vec = searchSinglePallets(point);
    if(point == 1) {
        
        for(auto &x : vec) {
            x.compSize = getCompSizeAt(x.palletX , x.palletY);
        }
    }
    sort(vec.begin() , vec.end() , simplePalletComp);
    vector<int>divisionTaken(5000);   ///Keeps track of which grid positions are assigned	///EXPERIMENT
    	
    for(int i =0 ; i < N ; i++ ) {	///
        if(!isAliveMy[i]) continue;	
        if(myPacBusy[i]) {
            divisionTaken[ gridDivisionId[ tgtX[i] ][tgtY[i] ] ] = 1;	
            cerr<<"pac "<<i<<" division= "<<gridDivisionId[ tgtX[i] ][tgtY[i]]<<" target= ("<<tgtY[i]<<" , "<<tgtX[i]<<")"<<endl;
        }
        	
    }
    int printCnt = 10;
    for(auto sp : vec) {
        
        int x = sp.palletX;
        int y = sp.palletY;
        int id = sp.pacId;
        
        if(printCnt ) {
            cerr<<point<<" -- "<<sp.dist<<" "<<sp.compSize<<" ----- "<<id<<" -- "<<y<<" "<<x<<" --> "<<myPacBusy[id]<<" , "<<cellTaken[x][y]<<endl;
            printCnt--;
        }
        
        if(cellTaken[x][y]) continue;
        
        if(divisionTaken[ gridDivisionId[x][y] ]) continue;
        
        if(myPacBusy[id]) {       ///UPGRADED
                
            int d13 = distanceBetween(nowX[id] , nowY[id] , x , y);
            int d12 = distanceBetween(nowX[id] , nowY[id] , tgtX[id] , tgtY[id]);
            int d23 = distanceBetween(tgtX[id] , tgtY[id] , x , y);
            if(d13 == d12 + d23) cellTaken[x][y] = 1;      
            continue;
        }

        tgtX[id] = x;
        tgtY[id] = y;
        
        ppp ret = getPalletToPac(x , y , nowX[id] , nowY[id] ,1);
        if(ret.uu.uu == -1) continue;
        
        nxtX[id] = ret.uu.uu;
        nxtY[id] = ret.uu.vv;
        
        jmpX[id] = ret.vv.uu;
        jmpY[id] = ret.vv.vv;
        
        bool ok = 1;
        
        for(int i =0 ; i < N ; i++) {   ///condition if another enemy is nearby on the path, NEEDED
            if(!enemVisible[i]) continue;
            
            if(  kills[ pacTypeAt[ nowX[id] ][ nowY[id] ] ] == pacTypeAt[ enemNowX[i] ][ enemNowY[i] ] ) {
                
                int d13 = distanceBetween(nowX[id] , nowY[id] , enemNowX[i] , enemNowY[i]);
                 
                if(d13 >=5 ) continue;      ///Don't change this value
                
                int d12 = distanceBetween(nowX[id] , nowY[id]  , nxtX[id] , nxtY[id]);
                int d23 = distanceBetween(enemNowX[i] , enemNowY[i] , nxtX[id] , nxtY[id]);
                
                if(d13 != d12 + d23) continue;
                
                if(id==0 && i == 1) cerr<<"uuuu "<<nowY[id]<<" "<<nowX[id]<<" -- "<<enemNowY[i]<<" "<<enemNowX[i]<<" -- "<<nxtY[id]<<" "<<nxtX[id]<<endl;
   
                cerr<<" cancelling "<<id<<" to "<<jmpY[id]<<" "<<jmpX[id]<<" , attacker = "<<i <<endl;
                ok = 0;
            }
        }
        if(ok) {
            cerr<<"assigning "<<id<<" to "<<y<<" "<<x<<endl;
            
            myPacBusy[id] = 1;
            cellTaken[x][y] = 1;
            divisionTaken[ gridDivisionId[x][y] ] = 1;
        }
    }
}

void reset() {      ///Updated
    
    for(int i =0 ; i < N ; i++) {
        switchTo[i] = "";
        myPacType[i] = "";
    }
    
    for(int i =0 ; i < n ; i++ ) {
        for(int j =0 ; j < m ; j++ ) {
            
            if(board[i][j] == -1) board[i][j] = -1;
            else if(board[i][j] == 2) board[i][j] = 2;
            else if(board[i][j] == 1) board[i][j] = 1;
            else board[i][j] = 0;
        }
    }
    
    for(int i =0 ; i < n ; i++ ) {
        for(int j =0 ; j < m ; j++ ) {
            pacTypeAt[i][j] = "";
        }
    }
    
    memset(myPacBusy , 0, sizeof myPacBusy);
    memset(cellTaken , 0 , sizeof cellTaken);
    memset(enemVisible , 0 , sizeof enemVisible);
    
    bigPalletTaken = 0;
    
    for(int i =0 ; i < N ; i++ ) {
        for(int j = N-2 ; j >=  0 ; j-- ) {
            myLastX[i][j+1] = myLastX[i][j];
            myLastY[i][j+1] = myLastY[i][j];

        }
    }
    
    
    memset(enemNowX , 0 , sizeof enemNowX);
    memset(enemNowY , 0 , sizeof enemNowY);
    memset(enemDead , 0 , sizeof enemDead);
    memset(enemPacBusy , 0 , sizeof enemPacBusy);
    
    totMovesDone++;
    
    ///NEW  Don't add new thing anything below this, do above if needed
    
    if(leafRevealed) return;
    
    if(gamePercentage >= LEAF_REVEAL ) {
        
        for(int i =0 ; i < n ; i++) {
            for(int j= 0 ; j < m ;j++ ) {
                if(leafMark[i][j] != 1)  continue;
                board[i][j] = 1;
                leafMark[i][j] = 0;
            }
        }
        leafRevealed = 1;
    }
    
}

void boardDivision() {	    ///NEW Test
    if(gamePercentage >= 110.00) {
        int cnt = 0;
        for(int i =0 ; i < n ; i++ ) {
            for(int j =0 ; j <m ; j++ ) {
                gridDivisionId[i][j]= ++cnt;
            }
        }
        return;
    }
    
    int myAlive = 0;
    for(int i =0 ; i < n ; i++ ) {
        for(int j = 0 ; j < m ; j++ ) {
            if(board[i][j] ==5) myAlive++;
        }
    }
    myAlive = max(myAlive , 1);
    
    
    int colDiv = max(myAlive , 4);
    int roDiv = 2;

    int rr = (n-1) / roDiv + 1;	
    int cc = m / colDiv ;
    int rem = m%colDiv;
    	
    for(int i =0 ; i < n ; i++ ) {	
        for(int j =0 ; j < m ; j++ ) {	
            gridDivisionId[i][j] = (i/rr )*colDiv +  (j/cc + 1);	
            if(j >= m - rem) gridDivisionId[i][j]--;         ///Last Added
        }	
    }	
    for(int i =0 ; i < n ; i++ ) {	
        for(int j =0 ; j < m ; j++ ) {	
            cerr<<gridDivisionId[i][j];	
        }	
        cerr<<endl;	
    }	
}	

void  updateGamePercentage() {      ///ok
    
    int totDot = 0 , remDot = 0;
    
    for(int i =0 ; i < n ; i++ ) {
        for(int j = 0 ; j < m ; j++ ) {
            if(original[i][j] == 0) totDot++;
            if(board[i][j] == 1 || board[i][j] == 10) remDot++;
        }
    }
    cerr<<remDot<<" "<<totLeaf<<endl;       ///Strange 
    gamePercentage = (totDot- remDot)*100.00/totDot;
}

void updateEnemLast() {     ///MEW

    for(int i =0 ; i < N ; i++ ) {
        if(enemDead[i]) continue;
        if(enemVisible[i]) {
            enemLastX[i] = enemNowX[i];
            enemLastY[i] = enemNowY[i];
        }
        else {
            enemLastX[i] = enemLastY[i] = -1;
        }
    }
}

int main()
{
    boardInput();
    kills["SCISSORS"] = "ROCK";
    kills["PAPER"] = "SCISSORS";
    kills["ROCK"] = "PAPER";
    while (1) {
            
        reset();        ///Resets Board
        int myScore , opponentScore;
        cin >> myScore >> opponentScore; cin.ignore();
        
        int visiblePacCount;
        cin >> visiblePacCount; cin.ignore();
        
        for (int i = 0; i < visiblePacCount; i++) {
                
            int pacId , x , y , speedTurnsLeft , abilityCooldown ;
            bool mine;
            string typeId; 
            cin >> pacId >> mine >> y >> x >> typeId >> speedTurnsLeft >> abilityCooldown; cin.ignore();
            
            if(typeId == "DEAD") {
                deadPacUpdate(pacId  , mine ,  x , y);
                continue;
            }
            if(mine) {
                myLastX[pacId][0] = nowX[pacId] = x;
                myLastY[pacId][0] = nowY[pacId] = y;
                
                coolDownRem[pacId] = abilityCooldown;
                
                isAliveMy[pacId] = 1;
                board[x][y] = 5;
                
                pacTypeAt[x][y] = myPacType[pacId] = typeId;
                
                mySpeedLeft[pacId] = speedTurnsLeft;
            }
            else {
                enemNowX[pacId] = x;
                enemNowY[pacId] = y;
                
                enemVisible[pacId] = 1;
                board[x][y] = -5;
                pacTypeAt[x][y] = typeId;
                
                enemSpeedLeft[pacId] = speedTurnsLeft;
            }
        }
        
        updateGamePercentage();
        cerr<<"Game Completed: "<<gamePercentage<<endl;
        
        boardDivision();        ///Divides the board for different different pacs
        

        myMemoryUpdateBeforeTakingPallet();     /// Updates Current Row and Column before taking new Pallets
        
        int visiblePelletCount; 
        cin >> visiblePelletCount; cin.ignore();
        for (int i = 0; i < visiblePelletCount; i++) {
            int x , y , value;
            
            cin >> y >> x >> value; cin.ignore();
            if(gamePercentage < LEAF_REVEAL && leafMark[x][y] && value == 1 ) continue;          ///NEW
            board[x][y] = value;
        }
        
        print();
        
        ///All Board are now complete, now updating my pacs

        updateMode();
        updateSpeed();  
        
        opponentMemoryUpdate();     ///Greedily set pallets visible at enemy site to 0
              
        boardSearch10();
        
        for(int i =0 ; i < N; i++ ) {
            
            if(isAliveMy[i] && !myPacBusy[i]) cerr<<" unassigned after 10 "<<i<<endl;
        }
        
        
        
        boardSearch(1);
        for(int i =0 ; i < N; i++ ) {
            
            if(isAliveMy[i] && !myPacBusy[i]) cerr<<" unassigned after 1 "<<i<<endl;
        }
        boardSearch(0);
        for(int i =0 ; i < N; i++ ) {
            
            if(isAliveMy[i] && !myPacBusy[i]) cerr<<" unassigned after 0 "<<i<<endl;
        }
        stayStill();
        
        resolveClash();
        
        
        updateTwo();
//        updateMirroring();       // Mirroring is done in boardSearch10();
        
        printMove();
        
        updateEnemLast();       ///updates enemLastXY
        
    }
    return 0;
}
