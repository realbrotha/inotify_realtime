# inotify_test 
## Env
- ubuntu 16.04.06 LTS
- clion 2020.01
- gcc 5

## BOOST 
- 적용중 

## Issue (Inotify 기능 문제)
- 몹시 불편하게도 디렉토리에 대한 리커시브한 기능이 없음 ..
- Watch Count .... 우회 하고싶음 .... inotify max event count 를 우회하고싶은데 마땅치가 않음 
- radix tree 로 매핑 nlogn stable 한 속도

## 결론 Fanotify는 엄청난 혁신, mount 단위로 적용하는것이 유효 




