c++에서도 구조체를 만들어서 클래스에 넣는게 편한가? 맞는건가?

구조체에 템플릿은 써지지만 그러면 typedef로 임의의 이름 짓기가 안됨

c언어 책에서는 임의 위치의 노드 삽입,삭제를 매개변수로 구조체를 받아서 실행, but cpp에서 private으로 캡슐화를 진행해서 int index로 위치를 제공받기로 결정
범위 이탈은 int size를 정의하고 예외처리할것
단순연결에서 



근데 operator<<는 friend로 선언은 됐지만 클래스 스코프 밖에서 정의되는 함수다(엄밀히는 클래스 정의부 안에 인라인으로 쓰긴 했지만, 언어 규칙상 friend 함수는 멤버 함수가 아니라 별개의 자유 함수). 일부 언어 서버는 friend 함수를 "이 클래스의 private 접근 권한이 있는 멤버"로 완벽하게 인식 못 하고, 그냥 일반 전역 함수처럼 취급해서 Node가 안 보이는(private라서 후보에서 빠지는) 상태로 분석하는 경우가 있다. 그래서 자동완성 목록 자체가 안 뜨거나 비어있게 나오는 거다.

중요한 건: 이건 IntelliSense가 놓친 거지, 실제 컴파일 규칙 위반이 아니다. friend 선언 자체가 "이 함수는 private 멤버 접근 권한을 가진다"는 뜻이라서, g++로 실제 컴파일하면 cur->data, cur->next 접근에 아무 문제 없다. IDE 정적 분석기의 한계고, 실제 컴파일러 동작이랑 IDE 툴링이 항상 100% 일치하진 않는다는 걸 지금 직접 겪은 거다.


인자를 private에 두고 수정 못하게 & 확인만 가능하도록 public const 메서드 


이중 연결리스트에서 복사생성자, 이동생성자 만들어서 단순연결에 적용해볼것  / 0814


## CustomListNode (단순 연결리스트) — 1차 리뷰

### 개요
- 대상: `CustomListNode<T>` — insert/remove/조회 인터페이스
- 리뷰 라운드: 총 8라운드 (컴파일 실패 2회 포함)
- 검증: g++ -Wall -Wextra 경고 0건, valgrind full leak-check 0 errors

---

### Critical (C++ 전환 특유 메모리/예외안전 이슈)
- 없음 (0건)
- 관찰: Queue/Stack 리뷰에서는 realloc→new[] 전환, 예외 안전 순서 등 C++ 특유 이슈가 다수였음.
  단순연결리스트는 C 원본 로직을 포인터 단위로 그대로 옮기는 성격이 강해
  전환 이슈보다 순수 포인터 조작 오류(Logic Bug) 비중이 압도적으로 높았음.
  → 자료구조 유형별 Critical/Logic Bug 비율 차이는 최종 리포트 분석 포인트로 기록.

### Logic Bug (언어 무관, 포인터/로직 순수 오답)
1. LB6 — remove_first(): throw 키워드/세미콜론 누락, 컴파일 자체 불가
2. LB7 — remove_first(): 빈 리스트 체크 조건 오류 (head->next==nullptr → head==nullptr)
3. LB8 — remove(): index==0 분기 return 누락 (UB)
4. LB9 — remove(): 루프 오프바이원 (i<index-1 → i < index)
5. LB10 — remove(): pre->next->next 대입, 실질 no-op → use-after-free
6. LB11 — insert(): 리팩토링 중 루프 시작값 리그레션 (i=1→i=0, nullptr 역참조)
7. LB12 — size_ 초기값 -1로 우회 시도, insert 경계조건 오작동
8. LB13 — insert(): `cur = newNode` (로컬 변수 재대입, 리스트 미연결) → 삽입 무효 + leak
9. LB14 — remove() tail 분기: `pre.next` 포인터에 `.` 사용, 컴파일 불가
10. LB15 — remove() tail 분기: size_-- 누락
11. LB16 — operator<<: 세미콜론 위치 오류로 문장 분리, 컴파일 불가
12. LB17 — operator<<: friend 함수 내 미한정 head 참조 (l.head 필요)
로직버그는 따로 md만들어서 몰아넣을듯?
### Design
- D1: index==0 처리 방식 → insert_first/remove_first를 private 헬퍼로 분리, 위임 구조로 확정
- D2: insert/remove 경계조건 분리
  - insert: `size_ < index` (index==size, append 허용)
  - remove: `size_ <= index` (단, index==0은 range 체크 제외 → remove_first가 빈 리스트 자체 처리)

### Performance
- 미측정. Track2 벤치마크 단계에서 std::list와 비교 예정.

### 검증
- g++ -std=c++17 -Wall -Wextra -O0 -g : 경고 0건
- valgrind --leak-check=full --show-leak-kinds=all : 13 allocs / 13 frees, 0 errors, no leaks possible
- 테스트 시나리오: 빈 리스트 예외, head 삽입 반복, append, 중간 삽입/삭제, 범위 초과 예외, 전체 제거 후 empty()

### 이월 항목
- Move semantics 미구현 (Rule of Three만 적용, Rule of Five 아님)
  → 이중연결리스트에서 prev 포인터 포함해 먼저 설계·검증 후, 단순연결리스트로 역이식 예정
  → 역이식 시에도 별도 valgrind 재검증 필수 (구조 유사성만으로 안전성 가정 금지)

### 커밋 메시지 초안
`[List] CustomListNode insert/remove Logic Bug 12건 수정, Design 확정, 컴파일/valgrind 검증 완료 (Performance 미착수, move semantics 이월)`