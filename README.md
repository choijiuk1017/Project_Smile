# Project Smile

> 생성형 AI가 플레이어의 게임 화면을 분석하여 현재 상황에 맞는 퍼즐 힌트를 제공하는 1인칭 공포 퍼즐 게임

플레이어가 직접 선택한 게임 화면을 캡처하여 AI 서버로 전송하고, 서버에서 생성한 장면 분석 결과와 단계별 힌트를 게임 UI에 표시하는 Unreal Engine 5 프로젝트입니다.

고정된 힌트 목록을 출력하는 방식이 아니라 현재 화면, 플레이어가 머문 구역, 힌트 요청 횟수 등의 정보를 종합해 상황에 맞는 힌트를 제공하는 것을 목표로 개발했습니다.

---

## 플레이 영상

[![Project Smile 플레이 영상](https://img.youtube.com/vi/kFRwyFXwx7A/0.jpg)](https://www.youtube.com/watch?v=kFRwyFXwx7A&t=126s)

- YouTube: https://www.youtube.com/watch?v=kFRwyFXwx7A&t=126s

---

## 프로젝트 개요

| 항목 | 내용 |
| --- | --- |
| 프로젝트명 | Project Smile |
| 장르 | 1인칭 공포 퍼즐 |
| 개발 형태 | 개인 프로젝트 |
| 개발 기간 | 2026.02 ~ 2026.07 |
| 담당 역할 | Unreal 클라이언트 및 AI 서버 파이프라인 구현 |
| 엔진 | Unreal Engine 5.5 |
| 개발 언어 | C++, Blueprint, Python |
| 플랫폼 | Windows |
| 통신 방식 | Unreal HTTP ↔ Flask REST API |

AI 서버는 별도의 저장소에서 관리합니다.

- AI Repository: https://github.com/choijiuk1017/Project_Smile_AI

---

## 기술 스택

### Client

- Unreal Engine 5.5
- C++
- Blueprint
- Enhanced Input
- UMG
- Render Target
- RHI
- ImageWrapper
- ImageWriteQueue
- HTTP
- JSON
- Behavior Tree
- Blackboard

### AI Server

- Python
- Flask
- PyTorch
- Transformers
- PEFT
- LangGraph
- LLaVA-OneVision
- Gemma
- Sentence Transformer
- RAG

---

# 핵심 구현 내용

- 1인칭 이동과 시점 조작을 구현했습니다.
- 상호작용 가능한 오브젝트의 공통 기반 클래스를 구성했습니다.
- 문서 열람, 아이템 조사, 문, 전등, 퓨즈 상자 등 퍼즐 오브젝트를 구현했습니다.
- 아이템 크기를 고려하는 격자형 인벤토리를 구현했습니다.
- 플레이어가 화면에서 분석 영역을 직접 선택할 수 있도록 구성했습니다.
- 선택한 화면 영역을 Render Target으로 캡처했습니다.
- 캡처한 이미지를 압축하여 Flask AI 서버로 전송했습니다.
- Unreal HTTP 모듈을 사용해 비동기 요청을 처리했습니다.
- AI 서버가 반환한 JSON을 파싱하여 조사 일지와 힌트 UI에 표시했습니다.
- AI 요청 중 Loading UI를 표시하고 실패 및 비정상 응답을 처리했습니다.
- Behavior Tree 기반 몬스터 추적과 탈출 이벤트를 구현했습니다.
- AI가 현재 화면에 존재하지 않는 오브젝트를 임의로 힌트에 포함하지 않도록 분석 범위와 출력 형식을 제한했습니다.

---

# 시스템 구성

```text
┌─────────────────────────────┐
│       Unreal Client         │
│                             │
│  화면 선택 및 장면 캡처     │
│  플레이어 상태 수집         │
│  이미지 데이터 압축         │
└──────────────┬──────────────┘
               │
               │ HTTP POST /predict
               │ Image + Player State
               ▼
┌─────────────────────────────┐
│       Flask AI Server       │
│                             │
│  LLaVA 이미지 분석          │
│  RAG 퍼즐 문서 검색         │
│  힌트 단계 결정             │
│  Gemma 조사 일지 생성       │
│  JSON 검증 및 Fallback      │
└──────────────┬──────────────┘
               │
               │ JSON Response
               ▼
┌─────────────────────────────┐
│       Unreal Client         │
│                             │
│  JSON 응답 파싱             │
│  조사 일지 저장             │
│  단계별 힌트 UI 출력        │
└─────────────────────────────┘
```

---

# 주요 시스템

## 1. 상호작용 시스템

게임에 등장하는 다양한 상호작용 오브젝트의 공통 기능을 `AInteractableActor`에 정의했습니다.

### 주요 상호작용 Actor

| 클래스 | 역할 |
| --- | --- |
| `AInteractableActor` | 상호작용 오브젝트의 공통 기반 클래스 |
| `ADocumentActor` | 문서 획득 및 열람 |
| `AItemActor` | 월드에 배치되는 아이템 |
| `ADoorActor` | 문 열기와 잠금 조건 처리 |
| `AFuseCaseActor` | 퓨즈 장착 및 전력 관련 퍼즐 |
| `ALightSwitchActor` | 조명 상태 전환 |
| `AInspectPreviewActor` | 아이템 확대 조사 화면 |
| `AAreaTriggerActor` | 플레이어가 진입한 구역 확인 |
| `AMonsterEscapeTrigger` | 몬스터 탈출 및 추격 이벤트 |

오브젝트마다 플레이어 탐색과 입력 처리를 반복해서 구현하지 않고 공통 상호작용 흐름을 기반 클래스에서 관리하도록 구성했습니다.

---

## 2. 화면 선택 시스템

`UCaptureSelection`은 플레이어가 AI에게 분석을 요청할 화면 영역을 직접 선택하는 Widget입니다.

### 기본 설정

| 설정 | 값 |
| --- | ---: |
| 기본 선택 크기 | 400 × 400 |
| 최소 선택 크기 | 100 × 100 |
| 최대 선택 크기 | 1500 × 1000 |
| 크기 변경 단위 | 20 Pixel |

### 주요 함수

```cpp
void GetSelectionRect(
    FVector2D& OutPosition,
    FVector2D& OutSize
) const;

void ResetSelection();

void UpdateSelectionVisual();
void ClampSelectionSize();

void AdjustVerticalSize(float Delta);
void AdjustHorizontalSize(float Delta);
```

### 처리 흐름

1. Widget을 열면 키보드 포커스를 설정합니다.
2. 선택 영역을 기본 위치와 크기로 초기화합니다.
3. 방향키 입력으로 선택 영역의 가로 또는 세로 크기를 변경합니다.
4. 최소 및 최대 크기를 벗어나지 않도록 `FMath::Clamp()`를 적용합니다.
5. `UCanvasPanelSlot`의 위치와 크기를 변경해 화면에 선택 영역을 표시합니다.
6. 확정된 위치와 크기를 화면 캡처 시스템에 전달합니다.

화면 전체를 항상 분석하지 않고 플레이어가 중요하다고 판단한 영역만 선택하도록 하여 불필요한 이미지 정보를 줄이고 분석 대상을 명확하게 했습니다.

---

## 3. 화면 캡처 및 이미지 변환

플레이어가 선택한 화면 영역을 Render Target으로 캡처한 뒤 서버로 전송할 수 있는 이미지 데이터로 변환합니다.

### 사용 모듈

```text
RenderCore
RHI
ImageWrapper
ImageWriteQueue
```

### 처리 흐름

```text
플레이어 분석 입력
    ↓
선택 영역 위치와 크기 확인
    ↓
현재 게임 화면 캡처
    ↓
Render Target Pixel Data 읽기
    ↓
이미지 크기 및 영역 보정
    ↓
PNG 또는 JPEG 형식으로 압축
    ↓
Byte Array 생성
    ↓
HTTP 요청 데이터에 추가
```

이미지 데이터를 디스크에 저장한 뒤 다시 읽는 방식 대신 메모리에서 바로 압축하고 HTTP 요청에 포함하도록 구성했습니다.

---

## 4. AI 서버 통신

Unreal Engine의 HTTP 모듈을 사용해 Flask 서버와 비동기로 통신합니다.

### 사용 모듈

```text
HTTP
Json
JsonUtilities
```

### 요청 정보

AI 서버에는 다음 정보를 전달할 수 있도록 구성했습니다.

- 현재 게임 화면 이미지
- 현재 구역 ID
- 힌트 요청 횟수
- 현재 구역 체류 시간
- 퍼즐 진행 상태

### 요청 흐름

```text
이미지 캡처 완료
    ↓
HTTP Request 생성
    ↓
Header와 Content-Type 설정
    ↓
이미지 및 플레이 상태 추가
    ↓
POST /predict 호출
    ↓
비동기 응답 대기
    ↓
HTTP 상태 코드 확인
    ↓
JSON 응답 파싱
    ↓
게임 UI 갱신
```

AI 요청 중에도 게임의 메인 스레드가 멈추지 않도록 비동기 응답 Callback에서 결과를 처리합니다.

---

## 5. JSON 응답 처리

AI 서버의 출력은 역할별 JSON 데이터로 구조화해 Unreal 클라이언트가 안정적으로 사용할 수 있도록 구성했습니다.

### 응답 활용 정보

- 이미지에서 확인한 장면 설명
- 관련 퍼즐 구역
- 조사 일지 제목
- 조사 일지 내용
- 플레이어에게 제공할 힌트
- 힌트 단계
- 검색된 퍼즐 정보
- 오류 및 Fallback 여부

### 처리 방식

1. HTTP 요청 성공 여부를 확인합니다.
2. 응답 문자열을 JSON 객체로 변환합니다.
3. 필요한 필드가 존재하는지 검사합니다.
4. 필드별 문자열과 숫자 데이터를 읽습니다.
5. 조사 일지와 힌트 데이터를 게임 상태에 저장합니다.
6. 관련 Widget에 결과를 표시합니다.
7. 파싱에 실패하면 오류 UI를 출력하고 잘못된 데이터를 적용하지 않습니다.

---

## 6. AI 분석 Loading UI

AI 추론은 일반적인 게임 로직보다 처리 시간이 오래 걸릴 수 있기 때문에 요청 상태를 사용자에게 표시합니다.

### 관련 Widget

- `UAnalysisLoadingWidget`

### 상태 처리

```text
분석 요청 시작
    ↓
Loading Widget 표시
    ↓
플레이어 중복 요청 제한
    ↓
HTTP 응답 대기
    ↓
성공 또는 실패 처리
    ↓
Loading Widget 제거
    ↓
힌트 또는 오류 UI 표시
```

이를 통해 AI가 응답하기 전까지 게임이 멈추거나 입력이 처리되지 않는 것처럼 보이는 문제를 줄였습니다.

---

## 7. 조사 일지 시스템

AI가 생성한 결과는 일회성 메시지로만 표시하지 않고 조사 일지에 기록합니다.

### 관련 Widget

| 클래스 | 역할 |
| --- | --- |
| `UFileJournalWidget` | 조사 일지 목록과 상세 내용 |
| `UPuzzleHintDialogue` | 현재 힌트 대화 출력 |
| `UDocumentWidget` | 게임 내 고정 문서 열람 |
| `UAnalysisLoadingWidget` | 분석 진행 상태 표시 |

조사 일지는 플레이어가 이전에 분석한 단서를 다시 확인하고 여러 퍼즐 정보를 연결할 수 있도록 구성했습니다.

---

## 8. 단계별 힌트 시스템

같은 퍼즐에서 힌트를 반복적으로 요청하면 점진적으로 더 구체적인 정보를 제공하도록 설계했습니다.

### 힌트 단계에 사용하는 정보

- 힌트 요청 횟수
- 해당 구역 체류 시간
- 현재 화면 분석 결과
- 현재 구역 ID
- RAG 검색 결과
- 퍼즐 진행 상태

### 예시

```text
1단계: 화면의 특정 오브젝트를 관찰하도록 유도
2단계: 오브젝트 간 관계를 추론하도록 방향 제시
3단계: 퍼즐 해결에 필요한 행동을 구체적으로 안내
```

처음부터 정답을 직접 제공하지 않고 플레이어가 스스로 퍼즐을 해결할 수 있도록 정보량을 조절했습니다.

---

## 9. 인벤토리 시스템

`UInventoryComponent`는 아이템이 차지하는 크기를 고려하는 격자형 인벤토리입니다.

### 기본 설정

```cpp
int32 GridWidth = 3;
int32 GridHeight = 3;
```

### 주요 함수

```cpp
bool TryAddItem(UItemData* ItemData);

bool CanPlaceItem(
    UItemData* ItemData,
    int32 StartX,
    int32 StartY
) const;

bool HasItemByID(FName ItemID) const;
bool RemoveItemByID(FName ItemID);

bool FindEmptyPosition(
    UItemData* ItemData,
    int32& OutX,
    int32& OutY
) const;

void PlaceItem(
    UItemData* ItemData,
    int32 StartX,
    int32 StartY
);

void RebuildGrid();
```

### 아이템 추가 흐름

1. 아이템의 가로와 세로 크기를 확인합니다.
2. 인벤토리의 각 위치에서 아이템이 들어갈 수 있는지 검사합니다.
3. 경계를 벗어나거나 이미 점유된 셀이 있으면 다음 위치를 검사합니다.
4. 배치 가능한 첫 번째 위치를 찾으면 아이템을 추가합니다.
5. 아이템이 차지하는 모든 셀에 아이템 인덱스를 기록합니다.
6. 인벤토리 변경 이벤트를 발생시켜 UI를 갱신합니다.

### 퍼즐 연동

`HasItemByID()`와 `RemoveItemByID()`를 사용해 문, 퓨즈 상자 등의 퍼즐 오브젝트가 특정 아이템의 보유 여부를 확인할 수 있도록 했습니다.

---

## 10. 아이템 데이터

아이템 정보는 `UItemData`를 통해 관리합니다.

### 관련 구조

- `UItemData`
- `FInventoryItemEntry`
- `FInventoryTypes`
- `UInventoryItem`
- `UInventorySlotWidget`
- `UInventoryWidget`
- `UItemInspectWidget`

아이템의 표시 정보와 실제 인벤토리 배치 상태를 분리해 UI와 게임 로직에서 동일한 데이터를 사용할 수 있도록 구성했습니다.

---

## 11. 몬스터 AI

몬스터는 AI Controller와 Behavior Tree를 기반으로 플레이어를 추적합니다.

### 관련 클래스

| 클래스 | 역할 |
| --- | --- |
| `AMonsterAIController` | Behavior Tree 실행 및 몬스터 제어 |
| `AMonsterCharacter` | 몬스터 상태와 이동 |
| `BTTask_FinishEscape` | 탈출 및 추격 이벤트 종료 |
| `AMonsterEscapeTrigger` | 몬스터 탈출 이벤트 실행 |

게임의 퍼즐 진행과 구역 Trigger에 따라 몬스터의 행동을 시작하거나 종료할 수 있도록 구성했습니다.

---

# 트러블 슈팅

## LLM이 화면에 존재하지 않는 오브젝트를 힌트에 포함하는 문제

### 문제

LLM이 게임 화면이나 RAG 데이터에 존재하지 않는 오브젝트와 해결 방법을 임의로 생성해 잘못된 힌트를 제공하는 문제가 발생했습니다.

### 해결

- LLaVA가 화면에서 실제로 확인할 수 있는 정보만 묘사하도록 분석 프롬프트를 제한했습니다.
- Gemma가 현재 장면과 RAG 데이터만을 근거로 결과를 생성하도록 규칙을 구성했습니다.
- LLM 출력을 역할별 JSON 데이터로 구조화했습니다.
- 필수 필드와 데이터 형식을 검사했습니다.
- 출력이 비정상적이거나 근거가 부족한 경우 Fallback 조사 일지를 사용했습니다.

### 결과

게임 데이터와 관련 없는 오브젝트나 해결 방법이 힌트에 포함되는 가능성을 줄이고 Unreal 클라이언트에서 AI 결과를 안정적으로 파싱할 수 있게 되었습니다.

### 배운 점

생성형 AI를 게임에 적용할 때는 결과 생성 자체보다 모델이 참고할 정보의 범위, 출력 형식, 검증 과정과 실패 처리를 함께 설계하는 것이 중요하다는 점을 배웠습니다.

---

## AI 응답 대기 중 게임이 멈춘 것처럼 보이는 문제

### 문제

AI 서버의 추론은 일반적인 게임 로직보다 처리 시간이 오래 걸리기 때문에 플레이어가 입력이 처리되지 않거나 게임이 멈춘 것으로 인식할 수 있었습니다.

### 해결

HTTP 요청을 비동기로 처리하고 요청 시작 시 Loading Widget을 표시했습니다.

요청이 완료되기 전에는 중복 분석 요청을 제한하고 성공, 실패, 파싱 오류에 따라 Loading 상태를 반드시 종료하도록 구성했습니다.

### 결과

AI 응답을 기다리는 동안 요청 상태를 명확하게 전달하고 중복 요청으로 인해 서버 부하와 UI 상태가 꼬이는 문제를 줄였습니다.

### 배운 점

AI 기능의 사용성은 추론 결과뿐만 아니라 대기 시간, 중복 입력, 실패 상태를 사용자에게 어떻게 전달하는지까지 포함해 설계해야 한다는 점을 배웠습니다.

---

## 해상도에 따라 화면 선택 범위가 달라지는 문제

### 문제

Widget의 화면 좌표와 Render Target의 Pixel 좌표가 서로 다른 기준을 사용해 해상도나 화면 비율에 따라 캡처 범위가 달라질 수 있었습니다.

### 해결

현재 Viewport 크기와 Render Target 크기의 비율을 계산해 Widget 좌표를 실제 캡처 Pixel 좌표로 변환했습니다.

선택 영역의 최소·최대 크기를 제한하고 캡처 범위가 Render Target을 벗어나지 않도록 보정했습니다.

### 결과

화면 해상도가 변경되어도 사용자가 선택한 영역과 실제 AI 서버에 전달되는 이미지 영역이 최대한 일치하도록 개선했습니다.

### 배운 점

화면 UI 좌표, Viewport 좌표와 Render Target 좌표를 사용할 때는 각 좌표계의 크기와 원점 기준을 명확히 구분해야 한다는 점을 배웠습니다.

---

# 주요 클래스

| 클래스 | 역할 |
| --- | --- |
| `AProject_SmileCharacter` | 플레이어 캐릭터 |
| `AMainPlayerController` | 인게임 입력과 UI 제어 |
| `ATitlePlayerController` | 타이틀 화면 입력 제어 |
| `AInteractableActor` | 상호작용 Actor 기반 |
| `ADocumentActor` | 문서 상호작용 |
| `ADoorActor` | 문과 잠금 조건 |
| `AFuseCaseActor` | 퓨즈 퍼즐 |
| `AItemActor` | 월드 아이템 |
| `ALightSwitchActor` | 조명 상호작용 |
| `AInspectPreviewActor` | 아이템 조사 미리보기 |
| `UInventoryComponent` | 격자 인벤토리 |
| `AMonsterAIController` | 몬스터 AI |
| `AMonsterCharacter` | 몬스터 캐릭터 |
| `UCaptureSelection` | 화면 분석 범위 선택 |
| `UAnalysisLoadingWidget` | AI 요청 상태 표시 |
| `UFileJournalWidget` | 조사 일지 |
| `UPuzzleHintDialogue` | AI 힌트 출력 |
| `UItemInspectWidget` | 아이템 확대 조사 |
| `UDocumentWidget` | 문서 열람 |
| `UInventoryWidget` | 인벤토리 UI |

---

# 프로젝트 구조

```text
Source/Project_Smile/
├─ Actor/
│  ├─ DocumentActor.*
│  ├─ DoorActor.*
│  ├─ FuseCaseActor.*
│  ├─ InspectPreviewActor.*
│  ├─ InteractableActor.*
│  ├─ ItemActor.*
│  └─ LightSwitchActor.*
│
├─ BT/
│  └─ BT_Task/
│     └─ BTTask_FinishEscape.*
│
├─ Character/
│  ├─ MonsterAIController.*
│  └─ MonsterCharacter.*
│
├─ Component/
│  └─ InventoryComponent.*
│
├─ Data/
│  ├─ InventoryTypes.*
│  └─ ItemData.*
│
├─ PlayerController/
│  ├─ MainPlayerController.*
│  └─ TitlePlayerController.*
│
├─ Trigger/
│  ├─ AreaTriggerActor.*
│  └─ MonsterEscapeTrigger.*
│
├─ Widget/
│  ├─ AnalysisLoadingWidget.*
│  ├─ CaptureSelection.*
│  ├─ DocumentWidget.*
│  ├─ FileJournalWidget.*
│  ├─ InteractionText.*
│  ├─ InventoryItem.*
│  ├─ InventorySlotWidget.*
│  ├─ InventoryWidget.*
│  ├─ ItemInspectWidget.*
│  ├─ PuzzleHintDialogue.*
│  └─ TitleWidget.*
│
├─ Project_SmileCharacter.*
├─ Project_SmileGameMode.*
└─ Project_Smile.Build.cs
```

---

# 사용 모듈

```csharp
PublicDependencyModuleNames.AddRange(new string[]
{
    "Core",
    "CoreUObject",
    "Engine",
    "InputCore",
    "EnhancedInput",
    "RenderCore",
    "RHI",
    "ImageWrapper",
    "ImageWriteQueue",
    "HTTP",
    "Json",
    "JsonUtilities",
    "UMG",
    "Slate",
    "SlateCore",
    "AIModule",
    "GameplayTasks",
    "NavigationSystem"
});
```

---

# 실행 방법

## 요구 환경

- Unreal Engine 5.5
- Visual Studio 2022
- Windows 10/11
- Python AI Server
- Git LFS

## 클라이언트 실행

```bash
git lfs install
git clone https://github.com/choijiuk1017/Project_Smile.git
cd Project_Smile
```

1. `Project_Smile.uproject`를 우클릭합니다.
2. `Generate Visual Studio project files`를 실행합니다.
3. Visual Studio에서 `Development Editor / Win64`로 빌드합니다.
4. `Project_Smile.uproject`를 실행합니다.
5. AI 서버의 주소와 Unreal 클라이언트의 요청 URL을 일치시킵니다.
6. AI 서버를 먼저 실행한 뒤 게임을 시작합니다.

> AI 서버가 실행되지 않은 상태에서도 게임은 실행할 수 있지만 화면 분석과 힌트 생성 기능은 정상적으로 동작하지 않습니다.

---

# 개발 과정에서 얻은 경험

- Unreal Engine 화면을 Render Target으로 캡처하는 방법
- Widget 좌표와 Render Target 좌표 변환
- 이미지 데이터를 메모리에서 압축하는 방법
- Unreal HTTP 비동기 통신
- JSON 데이터 검증 및 파싱
- AI 응답 대기와 실패 상태를 고려한 UI 설계
- 생성형 AI의 Hallucination을 줄이기 위한 입력 범위 제한
- AI 출력 형식 구조화 및 Fallback 처리
- 격자형 인벤토리 구현
- Behavior Tree 기반 몬스터 이벤트 구현
- Unreal 클라이언트와 Python AI 서버 연동

---

# Repository

- Client: https://github.com/choijiuk1017/Project_Smile
- AI Server: https://github.com/choijiuk1017/Project_Smile_AI
