# inotify를 이용한 실시간 이벤트 탐지 
## Env
- ubuntu 16.04.06 LTS
- clion 2020.01
- gcc 5
- BOOST 1.66.0 (소스코드, 라이브러리 꾸겨넣었음)

## Inotify Issue
- 몹시 불편하게도 inotify는 디렉토리에 대한 리커시브한 기능이 없음.
- Watch Count 사이즈 제한 있으며 default 사이즈가 매우작다 8142 정도 (inotify max event count를 sys명령어로 통제가능)
- fanotify 처럼 pid 가 날라오지 않아서 해당 파일의 예외처리 작업이 별도로 들어가야된다. (/proc/{pid}/fd 이쪽을 긁는게 가장 간단함)
- fd 정보가 없기때문에 파일 blocking 처리는 못할듯
- 이벤트 구조체 내부에 file name과 length만 날라오는데, full path를 얻기위해 add_watch가 된 list를 따로 유지가 필요하다. (move 이벤트가 오면 갱신이 필요, 들록된 내용은 유지가 되는듯함)

## Result
- 아래의 이미지 처럼 원하는 경로의 이벤트를 처리 가능함.
- 가능하다면 inotify 보다는 fanotify를 쓰자.

![main](1.jpg)


