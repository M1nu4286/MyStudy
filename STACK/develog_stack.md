# 자료구조 C→C++ 변환 devlog — 스택

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


## Critical 5 : 복사 생성자 / 복사 대입 연산자 부재

### 배경
CustomStack에 소멸자를 직접 정의(delete[] data)한 순간, 컴파일러가 자동 생성하는
기본 복사 생성자/대입 연산자는 얕은 복사(shallow copy)를 한다는 문제가 남는다.

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

### 구현 — 복사 대입 연산자
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









새로 알게된 c++ 표준클래스 
std::string 
c에서 char name[100](고정배열)나 char*(포인터)로 관리하던 걸 동적배열 및 길이,크기 연산 용이
소멸자 포함해서 free()안써도 누수걱정X

std::move
n을 가리


std::outofrange




stack_unwind_test
정순으로 생성후 역순으로 소멸되야하는데 exit쓰는순간 pop에서 끝나버리고 나머지 정상 소멸 X