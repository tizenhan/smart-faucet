
## I.o.T ( Initialize On Tizen )팀명 및 팀원
* 박신호 (기술조언 ,개발,디버깅, 설계)
* 한민수 (발표, 개발, 디버깅, 설계,기획,디자인)
* 한은총 ( 기획, 디자인 ) 


# ***Smart Faucet by I.o.T (스마트 수도꼭지)*** 

## 프로젝트 배경 혹은 목적
* 반신욕을 위한 물을 원하는 시간,온도,양에 맞추어 미리 준비해 놓자.
   *"특히 추운 겨울, 피곤에 지친하루를 보내고  집에 돌아온 후 수도꼭지 
     틀고 반신욕을 위한 따뜻한 물 받아지는것 지켜보며 기다리기 힘들다!"
*  분유, 이유식 먹고 배변하고 하루 수십번 씻겨야 하는 아기를 위한 
     따뜻한 물, 아기와 떨어지지 않고 마음 편히 준비할 수 있다.! 


## 타이젠 오픈소스에 컨트리뷰션한 내역
***반영 완료, [[적외선 온도센서](https://review.tizen.org/gerrit/#/c/apps/native/rcc/+/213558/)]***
***리뷰 중, [[초음파 거리센서](https://review.tizen.org/gerrit/#/c/apps/native/rcc/+/214042/)]***
***리뷰 중, [[릴레이제어](https://review.tizen.org/gerrit/#/c/apps/native/rcc/+/214279/)]***

## 파일 리스트  
 * inc/resource/resource_mcu90615_internal.h  	
 * inc/resource/resource_mcu90615.h 
 * inc/resource/resource_relay_internal.h 
 * inc/resource/resource_relay.h 	
 * inc/resource/resource_ultrasonic_jsn_sr04t_internal.h  
 * inc/resource/resource_ultrasonic_jsn_sr04t.h 
 * src/resource/resource_mcu90615.c 
 * src/resource/resource_relay.c 
 * src/resource/resource_ultrasonic_jsn_sr04t_internal.c  
 * src/smart-faucet.cpp 


## 코드 기여자  
 * inc/resource/resource_mcu90615_internal.h  한민수	
 * inc/resource/resource_mcu90615.h 한민수
 * inc/resource/resource_relay_internal.h 박신호
 * inc/resource/resource_relay.h 박신호	
 * inc/resource/resource_ultrasonic_jsn_sr04t_internal.h 한민수 
 * inc/resource/resource_ultrasonic_jsn_sr04t.h 한민수
 * src/resource/resource_mcu90615.c 한민수
 * src/resource/resource_relay.c 박신호
 * src/resource/resource_ultrasonic_jsn_sr04t_internal.c 한민수 
 * src/smart-faucet.cpp 한민수


## 보드
  * RPI3 1 : [[이미지 분석 및 모션센서 연동](github.com/tizenhan/smart-camera)]
  * RPI3 2 : [[온도,거리센서 연동](github.com/tizenhan/smart-faucet)]

## 구현사항
  * GPIO / UART 사용
  * 누비슨 클라우드 사용
  * 카메라 사용
  * 이미지 분석 기능 사용
