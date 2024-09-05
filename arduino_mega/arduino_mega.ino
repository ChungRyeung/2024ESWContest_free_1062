#include <Wire.h>

int array[6] = { 0, 0, 0, 0, 0, 0 };
int fire[6] = { 0, 0, 0, 0, 0, 0 };

#define INF 9999
#define NODE_COUNT 12

const int nodes[NODE_COUNT] = { '1', '2', '3', '4', '5', '6', 'A', 'B', 'C', 'x', 'y', 'z' };
int dist[NODE_COUNT][NODE_COUNT];
int next[NODE_COUNT][NODE_COUNT];
bool isFire[NODE_COUNT];
bool isfireflag = false;

int graph[NODE_COUNT][NODE_COUNT] = {
  //  1, 2, 3, 4, 5, 6, A, B, C, x, y, z
  { 0, 207, INF, INF, INF, 410, 100, INF, INF, INF, INF, INF },  // 1
  { 207, 0, 275, INF, INF, INF, INF, INF, INF, INF, 100, INF },  // 2
  { INF, 275, 0, 385, INF, INF, INF, INF, INF, INF, INF, 100 },  // 3
  { INF, INF, 385, 0, 287, INF, INF, 100, INF, INF, INF, INF },  // 4
  { INF, INF, INF, 287, 0, 316, INF, INF, 100, INF, INF, 100 },  // 5
  { 410, INF, INF, INF, 316, 0, INF, INF, INF, 100, INF, INF },  // 6
  { 100, INF, INF, INF, INF, INF, 0, INF, INF, INF, INF, INF },  // A
  { INF, INF, INF, 100, INF, INF, INF, 0, INF, INF, INF, INF },  // B
  { INF, INF, INF, INF, 100, INF, INF, INF, 0, INF, INF, INF },  // C
  { INF, INF, INF, INF, INF, INF, INF, INF, INF, 0, INF, INF },  // x
  { INF, 100, INF, INF, INF, INF, INF, INF, INF, INF, 0, INF },  // y
  { INF, INF, 100, INF, INF, INF, INF, INF, INF, INF, INF, 0 }   // z
};

int isFireArr[] = { 0, 0, 0, 0, 0, 0 };  // 예시 데이터: '3'과 '6' 노드에서 화재 발생
int angle_Arr[6] = { 180, 90, 180, 90, 180, 90 };

void initializeMatrix() {
    for (int i = 0; i < NODE_COUNT; i++) {
        for (int j = 0; j < NODE_COUNT; j++) {
            dist[i][j] = graph[i][j];
            if (graph[i][j] != INF && i != j) {
                next[i][j] = j;
            } else {
                next[i][j] = -1;  // INF이거나 자기 자신으로의 경로일 경우 -1로 설정
            }
        }
        isFire[i] = false;
    }
}

void floydWarshall() {
    for (int k = 0; k < NODE_COUNT; k++) {
        if (isFire[k]) continue;  // 화재가 발생한 노드는 건너뜀
        for (int i = 0; i < NODE_COUNT; i++) {
            if (isFire[i]) continue;
            for (int j = 0; j < NODE_COUNT; j++) {
                if (isFire[j]) continue;
                if (dist[i][j] > dist[i][k] + dist[k][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                    next[i][j] = next[i][k];
                }
            }
        }
    }
}

String constructPath(int u, int v) {
    if (next[u][v] == -1) {
        return "No path";
    }
    String path = "";
    while (u != v) {
        path += char(nodes[u]);
        path += " -> ";
        u = next[u][v];
    }
    path += char(nodes[v]);
    return path;
}

String findEscapeRouteForNode(int nodeIndex) {
    Serial.print("Entering findEscapeRouteForNode for node ");
    Serial.println(nodeIndex);

    int exits[] = { 6, 7, 8 };  // 'A', 'B', 'C'
    int shortestDist = INF;
    int bestExit = -1;

    for (int j = 0; j < 3; j++) {
        int exitNode = exits[j];
        if (next[nodeIndex][exitNode] != -1) {
            int distance = dist[nodeIndex][exitNode];
            if (distance < shortestDist) {
                shortestDist = distance;
                bestExit = exitNode;
            }
        }
    }

    if (bestExit != -1) {
        String result = "Node ";
        result += char(nodes[nodeIndex]);
        result += ": ";
        result += constructPath(nodeIndex, bestExit);
        Serial.print("Exiting findEscapeRouteForNode with result: ");
        Serial.println(result);
        return result;
    } else {
        String result = "Node " + String(char(nodes[nodeIndex])) + ": No safe escape route found!";
        Serial.print("Exiting findEscapeRouteForNode with result: ");
        Serial.println(result);
        return result;
    }
}

char getNextStepChar(String route) {
    int arrowIdx = route.indexOf("->");
    if (arrowIdx != -1 && arrowIdx + 2 < route.length()) {
        return route.charAt(arrowIdx + 3);  // "->" 뒤의 첫 문자 반환
    } else {
        return 'N';  // "N"은 경로가 없음을 의미하도록 함
    }
}

void setFireNodes(int fireNodes[], int size) {
    for (int i = 0; i < size; i++) {
        int nodeIndex = fireNodes[i];
        if (nodeIndex >= 0 && nodeIndex < NODE_COUNT) {
            isFire[nodeIndex] = true;
        }
    }
}

int Calculation_Escape_Angle(int CurrentNode, char NextNode) {
    int angle = 0;
    switch (CurrentNode) {
        case 0:
            if (NextNode == 'A') {
                angle = 0;
            } else if (NextNode == '6') {
                angle = 90;
            } else if (NextNode == '2') {
                angle = 180;
            }
            break;

        case 1:
            if (NextNode == '1') {
                angle = 0;
            } else if (NextNode == 'y') {
                angle = 90;
            } else if (NextNode == '3') {
                angle = 180;
            }
            break;

        case 2:
            if (NextNode == '4') {
                angle = 0;
            } else if (NextNode == '2') {
                angle = 90;
            } else if (NextNode == 'z') {
                angle = 180;
            }
            break;

        case 3:
            if (NextNode == '5') {
                angle = 0;
            } else if (NextNode == '3') {
                angle = 110;
            } else if (NextNode == 'B') {
                angle = 180;
            }
            break;

        case 4:
            if (NextNode == 'C') {
                angle = 0;
            } else if (NextNode == '6') {
                angle = 30;
            } else if (NextNode == '4') {
                angle = 150;
            } else if (NextNode == 'w') {
                angle = 180;
            }
            break;

        case 5:
            if (NextNode == 'x') {
                angle = 0;
            } else if (NextNode == '1') {
                angle = 110;
            } else if (NextNode == '5') {
                angle = 180;
            }
            break;

        default:
            break;
    }

    return angle;
}

void setFireNodesFromArr(int fireArray[], int arraySize) {
    int fireNodes[arraySize];
    int fireCount = 0;

    for (int i = 0; i < arraySize; i++) {
        if (fireArray[i] == 1) {
            fireNodes[fireCount++] = i;
        }
    }

    setFireNodes(fireNodes, fireCount);
}

void EvacuationRouteCalculation(int isFireArr[], int angle_Arr[]) {
    int arraySize = 6;  // 크기를 명시적으로 지정

    setFireNodesFromArr(isFireArr, arraySize);
    floydWarshall();

    int nodeNumber = 0;
    while (nodeNumber < arraySize) {
        if (isfireflag == 0) {
            angle_Arr[nodeNumber] = 10;  // 화재가 발생하지 않은 경우 각도 10도로 설정
        } else {
            String escapeRoute = findEscapeRouteForNode(nodeNumber);
            char nextStep = getNextStepChar(escapeRoute);
            angle_Arr[nodeNumber] = Calculation_Escape_Angle(nodeNumber, nextStep);
        }
        nodeNumber++; // 다음 노드로 이동
    }
}

void setup() {
    Serial.begin(9600);
    Wire.begin();  // I2C 마스터 시작

    initializeMatrix();
    floydWarshall();
}

void loop() {
    for (int address = 1; address <= 6; address++) {  // 슬레이브 주소 1, 2, 3, 4, 5, 6
        Wire.requestFrom(address, 4);                   // 슬레이브로부터 4바이트 요청

        unsigned long received = 0;
        int byteCount = 0;

        while (Wire.available() && byteCount < 4) {
            byte b = Wire.read();
            received |= ((unsigned long)b << (8 * byteCount));
            byteCount++;
        }

        if (byteCount == 4) {
            Serial.print(received);
            Serial.print(',');
        }
        fire[address - 1] = (received / 10000) % 10;
    }
    Serial.println();

    for (int j = 0; j <= 5; j++) {
        isFireArr[j] = fire[j];
        if (fire[j] == 1) {
          isfireflag = true;
        }
    }

    EvacuationRouteCalculation(isFireArr, angle_Arr);

    for (int address = 1; address <= 6; address++) {
        Wire.beginTransmission(address);
        Wire.write(angle_Arr[address - 1]);
        Wire.endTransmission();
    }

    delay(1000);  // 1초마다 읽기
}
