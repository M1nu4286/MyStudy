# 자료구조 C→C++ 변환 devlog — 스택

# 1차 리뷰 (2026-07-28)

## Critical 1: exit() → 예외처리 전환 (동적배열 스택)

### 원본 (C)
if (is_empty(s)) {
    fprintf(stderr, "스택 공백 에러\n");
    exit(1);
}
### 문제
- exit()는 콜스택에 쌓인 로컬 객체의 소멸자를 호출하지 않고 프로세스를 즉시 종료함
- C++은 스코프를 벗어날 때 소멸자가 반드시 불린다는 전제(RAII)로 설계됨
- exit()가 이 전제를 깨버림 → 락(lock_guard), 파일 핸들 등이 정리 안 된 채 프로세스 종료

### 전환 (C++, 검증용 파일: stack_critical_1_2.cpp)
throw std::out_of_range("스택이 비어있음");

### 검증 (완료)
- ResourceGuard 클래스로 소멸자에 로그 삽입 후 실제 컴파일/실행
- throw 버전: g2 → CustomStack → g1 순서(생성 역순)로 소멸자 호출 확인
- exit(1) 버전(대조군): 소멸자 로그 전혀 안 찍힘, 프로세스 즉시 종료 확인
-> 이를 통해 예외 처리를 이해함

-----

## Critical 2: 소멸자 누락 (RAII)
### 배경
원본 C: main()에서 수동 free(s.data) 호출 — 사용자가 까먹으면 누수
### 문제
소멸자 없으면 스코프 탈출 시 자원 해제가 보장 안 됨. 예외 도중 정상 흐름을 못 타면 free() 호출 라인 자체를 못 지나칠 수 있음
### 구현
~CustomStack() { delete[] data; }
### 검증 (완료)
valgrind --leak-check=full 실행 결과, HEAP SUMMARY: in use at exit: 0 bytes in 0 blocks, all heap blocks were freed — 소멸자가 스코프 탈출 시 정상 호출되어 data가 해제됨을 확인.

-----

## Critical 3: realloc 실패 시 원본 포인터 유실

### 배경
realloc은 malloc 계열로 할당된 메모리에만 사용 가능하며, 실패 시 원본 포인터를 잃어버림

### 문제
s->data = realloc(...) 형태로 반환값을 바로 덮어쓰면, 실패 시 NULL 대입 + 기존 메모리 leak

### 구현 (CustomStack.h)
new T[]로 새 배열 할당 → 값 복사 → 기존 delete[] → 교체 (실패해도 기존 data 안전)

### 검증 (실제 실행은 하지 않음)
실제 힙 고갈을 유도한 실행 검증은 시스템 리소스 소모가 크고 OOM 발생 가능

1. new T[newCapacity]에서 실패하면 그 즉시 bad_alloc이 던져지고 함수를 빠져나감
2. 이 시점까지 delete[] data(기존 해제), data = newData(교체) 둘 다 실행 전
3. 따라서 재할당 실패 시 기존 data/top/capacity는 손대지 않은 채 그대로 보존됨

realloc 방식은 반환값을 즉시 원본 포인터에 덮어써서 실패 시 기존 메모리를
잃어버리는 반면, new[]+복사 방식은 "새 자원 확보 → 성공 확인 후 → 기존 자원 교체"
순서를 강제하므로 이 문제가 구조적으로 발생할 수 없다.

### 결론
new[]+복사 방식은 realloc과 달리 실패 시 원본 포인터를 잃어버릴 위험이 구조적으로 없다.
(참고: 원래 별도 항목으로 뒀던 "오버플로우 처리 시 realloc 실패 체크 누락"도
이 구조 변경으로 동시에 해결됨 — new[]가 실패하면 언어 차원에서 즉시 예외가
던져지므로 "체크 누락"이라는 상황 자체가 성립하지 않음)

-----

## Critical 4: 템플릿 미적용
### 배경
원본 element = int 고정, std::stack<T>와 공정한 비교 불가
### 구현
template<typename T> class CustomStack
### 검증 (완료) — test_stack_critical4.cpp
- CustomStack<int>, <double>, <std::string> 각각 객체화하여 push/pop 정상 동작 확인
- std::string으로 확장(capacity *2) 로직까지 타입 무관하게 동작함을 확인
  (복사 생성자/대입 연산자가 int 전제로 잘못 짜였다면 string에서 드러났을 것 — 문제없음)

-----

## Critical 5: 복사 생성자 / 복사 대입 연산자 부재 (Rule of Three)

### 배경
CustomStack에 소멸자를 직접 정의(delete[] data)한 순간, 컴파일러가 자동 생성하는
기본 복사 생성자/대입 연산자는 얕은 복사를 한다는 문제가 남는다.

### 문제 재현
CustomStack<int> s1;
s1.push(1);
CustomStack<int> s2 = s1;  // 컴파일러 기본 복사 생성자 (얕은 복사)
// s1.data와 s2.data가 같은 주소를 가리킴
// s1, s2가 각각 스코프 벗어날 때 같은 메모리를 두 번 delete[] → double free (UB)

### 원칙
소멸자를 직접 만들어야 하는 클래스(= 리소스를 직접 관리하는 클래스)는
소멸자, 복사 생성자, 복사 대입 연산자 세 개를 전부 직접 정의해야 한다 (Rule of Three).
셋 중 하나가 필요하다는 건 셋 다 필요하다는 신호다.

### 구현 — 복사 생성자
CustomStack(const CustomStack& Other) {
    top = Other.top;
    capacity = Other.capacity;
    data = new T[capacity];
    for (int i = 0; i <= top; i++) data[i] = Other.data[i];  // 깊은 복사
}

### 구현 — 복사 대입 연산자 (1차 리뷰 당시 버전 — 2차 리뷰에서 순서 재수정됨, Critical 7 참고)
CustomStack& operator=(const CustomStack& Other) {
    if (this != &Other) {           // self-assignment 방어
        delete[] data;               // 기존 자원 해제 (안 하면 메모리 누수)
        top = Other.top;
        capacity = Other.capacity;
        data = new T[capacity];
        for (int i = 0; i <= top; i++) data[i] = Other.data[i];
    }
    return *this;
}

### 실수했던 부분 (기록해두는 이유: 같은 실수 반복 방지)
1. 복사 생성자에서 data = new T[capacity]만 하고 실제 값 복사 루프를 빼먹음
   → 배열은 만들어졌지만 내용은 초기화 안 된 쓰레기값
2. 대입 연산자에서 this == Other로 비교 시도 → 컴파일 에러
   this는 포인터, Other는 참조라 타입이 안 맞음 → this != &Other로 수정
3. 대입 연산자에서 return *this 누락 → 시그니처(CustomStack&)와 안 맞아 컴파일 에러

### 검증 (완료) — test_stack_critical5.cpp
$ ./stack_critical_5
s2: 2 1
s3: 2 1
s1: 2 1

$ valgrind --leak-check=full ./stack_critical_5
HEAP SUMMARY:
    in use at exit: 0 bytes in 0 blocks
    total heap usage: 7 allocs, 7 frees, 74,784 bytes allocated
All heap blocks were freed -- no leaks are possible
ERROR SUMMARY: 0 errors from 0 contexts

→ 복사 생성자/대입 연산자의 깊은 복사, self-assignment 방어가
  double free나 메모리 누수 없이 정상 동작함을 증명

-----
-----

# 2차 리뷰 (2026-07-29)

### 배경
1차 리뷰는 "정상 실행 경로"에서의 메모리 안전성(누수, double free)에 집중했음. "연산 도중 예외가 발생했을 때의 안전성"
(강한 예외 보장, Strong Exception Guarantee)은 점검하지 않았다는 한계를 발견함.

## Critical 6: pop() 값 반환 시 복사 생성자 예외로 인한 데이터 증발

### 문제
return data[top--]; 에서 T의 복사 생성자가 힙 할당 실패 등으로
예외를 던지면, top은 이미 감소했는데 값은 호출자에게 전달되지 못함.
해당 원소는 스택에서도 나갔고 호출자에게도 안 넘어가 증발함
(int처럼 복사에 힙 할당이 없는 타입에서는 발생 불가능한 문제).

### 전환
void pop()
{
    if (is_empty())
        throw std::out_of_range("스택이 비어있음");
    top--;
}
값 조회는 peek()으로 완전히 분리 (std::stack과 동일한 인터페이스 관례).

### 부수 변경
peek()+pop() 인터페이스 전환에 따라 stack_critical_4.cpp,
stack_critical_5.cpp의 pop() 값 사용부 전부 peek()+pop()으로 수정.

### 검증 (완료)
재컴파일 및 실행 결과 기존과 동일한 값 출력 확인
(int/double/string peek 값, Rule of Three 복사/대입 결과 모두 일치)


-----
-----

## Design 1: peek()이 값 복사(T) 반환
### 문제
값을 엿보기만 하는 함수인데 무조건 T를 복사해서 반환함.
T가 크기가 큰 구조체나 긴 문자열이면 단순 조회만 해도 복사 비용 발생.

### 개선 방향 (미적용)
const T& 반환으로 변경 시 복사 없이 조회 가능.
그런데 이러면 push시 is_full로 배열 크기 2배로 늘릴 때 기존배열 없애고 새배열 만드는 과정에서 댕글링 참조 발생

-----

# Performance (구조는 맞지만 효율성 개선 여지가 있는 지점)

## Performance 1: push()의 move semantics 부재
### 문제
void push(const T &item) — 임시 객체(rvalue)를 넘겨도 const T&가 받아
함수 내부에서 lvalue로 취급되어 항상 복사 대입이 발생함.
실제로 Watched 클래스로 재현: 임시 객체 push 시 이동 생성자가 아닌
복사 대입이 호출됨을 실행으로 확인.

### std::stack과의 차이
std::stack은 push(const T&)와 push(T&&) 두 오버로드를 제공하여
임시 객체는 이동 생성자/이동 대입으로 처리 — 불필요한 deep copy 회피.

### 적용 범위 판단
기본 자료형(int, double 등)은 복사/이동 비용이 사실상 동일하여
move semantics 부재가 성능에 영향을 주지 않음. 힙 자원을 보유한
타입(std::string, 커스텀 객체 등)에서만 의미 있음.

→ Track 2 벤치마크 스코프 정리:
  - CustomStack<int> vs std::stack<int>: 재할당 전략/캐시 미스 비교 목적
  - CustomStack<std::string> vs std::stack<std::string>:
    위 항목 + move semantics 부재로 인한 복사 오버헤드까지 관찰 가능

### 결정
push(T&&) 오버로드는 지금 추가하지 않고, string 벤치마크 결과에서
복사 오버헤드가 실제로 관찰되면 그때 추가 후 개선 전/후 비교하는 방식으로 진행.

## Performance 2: push() 재할당 시 std::move 미사용
### 문제
용량 초과로 배열 재할당 시 newData[i] = data[i]로 복사함.
기존 배열(data)은 이 루프 직후 delete[]로 버려질 것이므로,
굳이 복사할 필요 없이 std::move(data[i])로 이동하면 됨.

### 개선 방향 (미적용)
newData[i] = std::move(data[i]); 로 변경 시 힙 자원 보유 타입에서
재할당 속도 개선 예상 (가설 — 벤치마크로 검증 필요).

-----
-----


# 새로 알게된 C++ 표준 클래스/개념

## std::string
C에서 char name[100](고정배열)이나 char*(포인터)로 관리하던 걸
동적배열 및 길이/크기 연산이 용이하게 대체. 소멸자 포함해서
free() 안 써도 누수 걱정 없음.

## std::move
리소스(힙 버퍼 등)의 소유권을 복사 없이 이전시키는 것. 임시 객체나
더 이상 안 쓸 값을 복사하지 않고 그대로 넘겨서 불필요한 deep copy를 없앤다.
(미완성 — Performance 1, 2 실제 구현 시 다시 정리 예정)

## std::out_of_range
<stdexcept> 헤더의 표준 예외 클래스. "인덱스나 범위를 벗어난 접근"을
표현하는 용도로 표준에서 이미 정의해둔 타입. std::vector::at(),
std::string::at() 등도 이 예외를 던지므로, 직접 만든 커스텀 예외
클래스보다 STL과 같은 언어로 말할 수 있다는 장점이 있음.

## Stack Unwinding (검증 완료 — Critical 1 참고)
정순으로 생성 후 역순으로 소멸되어야 하는데, exit() 사용 시
pop()에서 프로세스가 즉시 끝나버려 나머지 소멸자가 정상 호출되지 않음.
(Critical 1의 검증 내용과 동일 — 상세 내용은 Critical 1 참고)