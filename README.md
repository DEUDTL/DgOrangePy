# Orange 사용법

## 사용 준비 및 사전 테스트

### Python으로 Orange 설치

```powershell
PS C:\> pip install PyQt5 PyQtWebEngine
PS C:\> pip install orange3
```

### Python으로 실행해보기

다음 python 코드를 예를 들어 "test.py"란 이름으로 만듭니다

```python
import Orange
import pickle
import pandas as pd

print('Orange 시작')
model = pickle.load(open('Modelexport.pkcls', 'rb')) #모델 불러오기
test = Orange.data.Table('Modeltest.tab'); #테스트 데이터 불러오기
pred_ind = model(test) #모델에 테스트 데이터 입력 결과
Pred = pd.DataFrame(model.domain.class_var.str_val(i) for i in pred_ind)  # 예측 출력된 값
Pred_list = [model.domain.class_var.str_val(i) for i in pred_ind]  # 예측 출력된 값
prob = pd.DataFrame(model(test, model.Probs))  # 예측 정확도
prob['prediction'] = Pred
print('결과 CSV로 저장')
prob.to_csv('zz.csv')
print('Orange 끝')
```

실행 해봅니다. 먼저 **Modelexport.pkcls**와 **Modeltest.tab**이란 데이터가 있는 지도 확인합니다.

```powershell
PS C:\> python test.py
```

그리고, zz.csv 파일이 만들어졌나 확인합니다.

## C/C++ 에서 사용

*LaunchOrange.cpp*와 *LaunchOrange.h*를 프로젝트에 등록합니다. 윈도우 헤더 이외에 표준 C++14만 사용했으므로 MSV, CLANG에서 모두 사용할 수 있습니다.

### 테스트

위의 python 테스트와 마찬가지로 **Modelexport.pkcls**와 **Modeltest.tab** 를 준비해야 합니다. 또한, 용도에 따라 다르지만 독립실행형(standalone)으로 만들면 다음과 같은 코드가 됩니다.

각 줄의 설명은 주석으로 표현합니다.

```c++
#include <iostream> 
#include "LaunchOrange.h" // LaunchOrange 함수가 들어있는 C헤더

int main()
{
    // 파이선 실행파일을 등록합니다. 기본 "python.exe"이므로 여기서는 주석처리 했습니다
    //SetOrangePython("python.exe");

    // 결과값을 받을 UTF8 문자열 포인터 입니다. 널포인터로 초기화 해주세요.
    char* csv = nullptr;    

    // LaunchOrange를 호출합니다. 인수 문자열은 반드시 유니코드입니다!!!
    const auto ret = LaunchOrange(
        L"Modelexport.pkcls",               // 모델 파일
        L"Modeltest.tab",                   // 데이터 파일
        &csv);                              // 결과 반환용
    );
    // 리턴값 ret은 int형으로 0이 아니면 오류입니다.
    if (ret != 0)
        std::cerr << "데이터를 가져올 수 없어요: " << ret;
    
    // 여기서 csv를 사용합니다
    // csv에는 UTF8의 여러줄의 쉼표로 구분된 csv형태의 문자열입니다

    // csv를 다 썼으면 메모리를 해제합니다. C에서도 사용할 수 있도록 malloc으로 할당되어 있기에 free로 해제합니다
    if (csv)
        free(csv);

    // 함수 main 종료
    return 0;
}
```

### 기타 사항

주석에 표시되어 있지만 반환용 인수 csv는 malloc할당입니다. 사용 후엔 *free*로 해제하세요. *delete*로 해제하면 안됩니다.

사실 필요한 파일은 *LaunchOrange.cpp* 뿐이므로 별도의 라이브러리로 만들지 않았습니다. 그 쪽이 갖다 사용할 때 더 불편하겠죠. 프로젝트나 make파일에 위에 코드 파일을 추가하세요.

