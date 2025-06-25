# DuoMech

## 1. 프로젝트 개요

# DuoMech

> 탑다운 협력 멀티플레이 퍼즐 게임  
> Unreal Engine 5 | C++ | LAN Networking | UMG

---

## 프로젝트 개요

**DuoMech**는 두 명의 플레이어가 각기 다른 역할을 수행하며, 협력을 통해 스테이지를 돌파하는 퍼즐 게임입니다. 
각 플레이어는 서로 다른 조작 방식으로 장애물을 피하고, 열쇠를 얻어 문을 여는 등의 역할 분담이 필요합니다. LAN 기반 멀티플레이 환경에서 함께 플레이할 수 있습니다.

---

## 주요 기능

### 플레이어 시스템
- Player1, Player2 역할 분리 (Role 기반 캐릭터 분기, Forward/Back, Left/Right)
- 각자 다른 장애물 회피 방향, HUD, 리스폰 위치 적용

### LAN 멀티플레이
- 언리얼 세션 생성 / 참가 기능 구현
- Server RPC / Multicast를 통한 클라이언트 간 상태 동기화

### 레이저 장애물 (DMLaserZone)
- 플레이어 위치 기반 레이저 생성 (8초 간격)
- Player1은 좌우 회피 / Player2는 앞뒤 회피
- 시작 트리거 진입 시 자동 활성화, 완료 트리거 진입 시 자동 비활성화

### 퍼즐 오브젝트 시스템
- 열쇠 상자(Box)와 문(Door) 연결
- 상호작용 시 서버에서 상태 제어 → 모든 클라이언트에 반영
- FindKey() 기반 연결 관리

### 스테이지 관리 (DMStageManager)
- 시작 트리거 진입 시: 플레이어가 죽을 수 있는 상태 + 레이저 활성화
- 완료 트리거 진입 시: 무적 상태 + 레이저 비활성화
- 각 스테이지별 상태 초기화 및 리스폰 처리 포함

### HUD 시스템
- Player1/Player2 각자 전용 UMG HUD (`WBP_DMPlayer1HUD`, `WBP_DMPlayer2HUD`)
- 방향키(WASD) 입력 시 이미지 색상 실시간 변경

---

## 기술 스택

| 분류            | 기술                                          |
| --------------- | --------------------------------------------- |
| Engine          | Unreal Engine 5.4                             |
| Language        | C++                                           |
| Networking      | Unreal Networking (RPC, Replication, Session) |
| UI              | UMG (Blueprint + C++)                         |
| Input           | Enhanced Input System                         |
| Version Control | Git / GitHub                                  |

---

