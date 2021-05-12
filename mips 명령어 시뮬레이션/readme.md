This program is made by
2015113163_박효상
2015117619 김지수

# C program simulating mips datapath and control

> 김지수
- lmplementing Mode and Output(printing result into txt file)
- Implementing Signal of control unit and ALU control signal
- Making test cases for testing this program

> 박효상
- Implementing basic structure of program
- Implementing functions and their connections
- Testing program


> init();
이 함수에선 레지스터, 인스트럭션 & 데이터 메모리, PC를 초기화 하고 runme.hex에서바이너리로 된 명령어를 읽어드려 인스터력션 메모리에 순서대로 나열한다.

> convert();
이 함수에선 레지스터 출력을 위해 각 번호가 어떤 레지스터를 의미하는지 스트링으로 변환한다.

> fetch();
이 함수에선 현제 PC를 인덱스로 가지는 레지스터에 저장된 명령어를 받아 리턴한다. 그후 j타입 명령어를 걸러서 j타입이면 pc를 수정하고 명령어 종류에 따라 부과적인 일을 수행한다.

> decode();
이 함수는 fetch에서 받아온 명령어를 받아 op 필드를 통해 어떤 명령어인지 판별하고 그에 따라 컨트롤 시그널을 할당하여 준다. 그 후 명령어의 각 rs, tr, rd, smt, fnct(or const, adress)  부분을 읽고 컨트롤 시그널에 맞추어 맞는 값과 레지스터에 write할 위치를 리턴한다.

> exe();
이 함수에선 디코드 단계에서 받은 필드를 인덱스로 하는 레지스터의 데이터, 명령어 뒷자리 16비트를 받아오고 컨트롤 시그널과 펑션필드를 알맞게 조합하여 ALU 시그널을 생성한다.
그 후 멀티플렉서로 선정한 알맞은 데이터들로 ALU 시그널에 맞는 연산을 실행하고 결과값을 리턴한다. 또한 두 데이터가 같으면서 컨트롤 시그널 branch가 1이라면 PC를 알맞은 값으로 변경한다.

> mem();
이 함수에선 exe 단계에서 받은 메모리의 주소와 메모리에 적을 데이터를 받아오고시그널에 따라 메모리에 값을 저장하거나 메모리에 있는 값을 리턴한다.

> wb();
이 함수에선 decode 단계에서 받아온 write할 위치와 alu에서 계산되어 나온 결과값, 저장할 데이터 값을 받아와 컨트롤 시그널 RegWrite가 1이라면 위치에 데이터 값을 저장한다.