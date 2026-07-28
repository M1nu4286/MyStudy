Prim 트리
🔴 Critical (실행 시 UB 발생)
#위치문제결과1selected[]초기화 없음쓰레기값으로 정점 skip2get_min_vertex()의 v모두 selected일 때 미초기화 반환UB3n, weight[][]세팅 수단 없음클래스 사용 자체 불가

🟡 Design (구조적 결함)
#문제원인4생성자 없음멤버 초기화 시점 미정의5INF = 1000가중치 ≥ 1000이면 로직 붕괴6#define 사용constexpr로 대체해야 타입 안전7get_min_vertex(int n)멤버 n 있는데 파라미터로 또 받음. 불일치 가능성

🟢 성능 (알고 쓰는가?)
#문제비고8get_min_vertex() O(V) → 전체 O(V²)우선순위 큐 쓰면 O(E log V)9인접행렬 고정 10,000칸sparse graph에서 메모리 낭비

한 줄 요약

Critical 3개가 잡히기 전까지 나머지는 논할 가치 없다. 1~3번 먼저 고쳐라.




Kruskal

🔴 Critical (실행 시 UB 및 메모리 오염 발생)#위치 / 문제원인 및 결과
1. insert_edge()정적 버퍼 오버플로우edges 배열 크기는 MAX_VERTICES (100)인데, 방어 코드는 2 * MAX_VERTICES (200)까지 허용함. 간선이 100개를 넘어가는 순간 인접한 힙/스택 메모리(다른 멤버 변수)를 오염시키고 Undefined Behavior(세그멘테이션 폴트 등) 발생.
2. UnionFind::unite()불필요한 중복 연산main 루프에서 이미 find()를 통해 루트 노드(uset, vset)를 찾아놓고, unite() 내부에서 똑같은 find()를 또 호출함. 대규모 그래프 탐색 시 CPU 사이클을 심각하게 낭비하는 주범.
3. main()명분 없는 동적 할당고정 크기 배열을 가진 객체를 굳이 new로 힙에 할당하여 메모리 단편화 위험을 키움. 스마트 포인터(std::unique_ptr)도 쓰지 않아 예외 발생 시 메모리 누수(Memory Leak) 확정.
🟡 Design (구조적 결함)#위치 / 문제원인 및 결과
4. MAX_VERTICES고정 매크로 사용#define 매크로 상수로 크기를 박아두어 유연성이 전혀 없음. 정점 수는 동적으로 받으면서 간선 배열은 정적으로 제한하는 모순된 구조. std::vector로 대체해야 함.
5. UnionFind트리 편중(Skewed) 위험Union-by-Rank(트리 높이 제어) 최적화가 누락되어, 데이터 입력 순서에 따라 트리가 사슬 형태로 길어짐. 최악의 경우 find() 연산 효율이 $O(\log N)$이 아니라 $O(N)$까지 떨어짐.
6. GraphType미사용 멤버 변수선언만 해두고 쓰지 않는 멤버 변수 capacity가 존재함. 메모리 레이아웃 구조상 불필요한 패딩이나 데이터 공간을 차지하는 군더더기 코드.
🟢 성능 (알고 쓰는가?)#위치 / 문제원인 및 결과
7. std::sort연산 병목간선 수가 정점 수에 비해 압도적으로 많은 밀집 그래프(Dense Graph)의 경우, 정렬 비용($O(E \log E)$)이 폭발함. 그래프의 특성을 고려하지 않고 크루스칼만 고집하는 것은 비효율적임.





스텍
#	항목
1	exit(1) → 예외 전환	RAII/스택 언와인딩 보장 위반 — 현재 진행 중
2	소멸자 누락	수동 free()/delete[]에 의존, RAII 없음
3	realloc 실패 시 원본 포인터 덮어쓰기	반환값 바로 대입 → 실패하면 기존 메모리 leak + data가 NULL 되어 dangling
4	템플릿 미적용	int 전용 → std::stack<T>와 공정 비교 불가
5	오버플로우 처리에서 realloc 실패 체크 누락	is_full() 체크 후 capacity *= 2 + realloc인데, 그 realloc 자체의 실패 가능성을 안 봄 (3번이랑 사실상 같은 지점, 다른 각도)



전체적인 부분
책 내용이 c언어 기반이라 코딩과정에서