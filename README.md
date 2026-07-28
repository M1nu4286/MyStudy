<<<<<<< HEAD
[그래프 파일 분할 & 빌드 시스템 - 2026.05.26]

## 깨달은 것

### 1. 헤더 분리의 목적
- .h : 클래스 선언만 (멤버 변수, 함수 시그니처)
- .cpp : 구현만
- .h를 여러 .cpp가 include → 동일한 클래스 정의를 중복 없이 공유
- .h에 iostream 넣으면 안 됨 → include한 모든 번역 단위에 딸려옴

### 2. 파일 분할 구조 (그래프 프로젝트 기준)
GraphMat.h / GraphList.h  ← 공통 선언
GraphMat.cpp / GraphList.cpp  ← 공통 구현
Bfs_Mat.cpp  ← #include "GraphMat.h" + bfs() + main
Dfs_Mat.cpp  ← #include "GraphMat.h" + dfs() + main
Bfs_List.cpp ← #include "GraphList.h" + bfs() + main
Dfs_List.cpp ← #include "GraphList.h" + dfs() + main

→ 인접 행렬/리스트 코드는 한 번만 작성, 탐색 알고리즘만 각자 구현

### 3. g++ -o vs Makefile
g++ -o : 명령을 손으로 직접 침. 파일 많아지면 관리 불가
Makefile : 빌드 규칙 등록 → make 한 번으로 전부 처리
           핵심 차이 = 의존성 추적
           GraphMat.h 수정 시 → 영향받는 파일만 자동 재컴파일
           안 바뀐 파일은 건드리지 않음

### 4. 확장 방향
Makefile → 나중에 CMake로 구조는 동일, 규모만 커짐
지금 Makefile 손으로 써보는 게 CMake 이해의 바닥




쉘정렬쓰는이유가 뭔지?


이상적인 해시 n mod j



키가 테이블 인덱스보다 클때
key ^ (key>>16) 비트 믹싱
이동/경계 폴딩

중간 제곱 함수
비트 추룰 기법
숫자 분석 기법

문자열일경우 아스키, 유니코드로 변환후 해싱`


해싱 코드들도 해더랑 실제구현부 분리
폴딩, 나눗셈같은 기능들 hash_fuction.h
실제구현부, 클래스 선언부, 밴치마크 비교 3개


충돌 및 오버플로우 발생시 해결방법







벤치마킹시 직접 구현과 라이브러리 사용에서 서로의 장단점이 있을텐데 이걸 비교해서 어느 상황에 뭐가 적절한지 찾기
