# inotify_test 
## Env
- ubuntu 16.04.06 LTS
- clion 2020.01
- gcc 5
- BOOST 1.66.0 (소스코드, 라이브러리 꾸겨넣었음)

## Issue 
- 몹시 불편하게도 inotify는 디렉토리에 대한 리커시브한 기능이 없음 ..
- Watch Count 사이즈 제한 있으며 default 사이즈가 매우작다 8000개정도 (inotify max event count를 sys명령어로 통제가능)
- radix tree 로 매핑 nlogn stable 한 속도

## 기타
- Fanotify는 엄청난 혁신.  

# 현재 코드 작업 계속 진행중 ~


