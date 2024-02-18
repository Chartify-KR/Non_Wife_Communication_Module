import 'dart:io';
import 'dart:async';
import 'dart:convert';

Future<void> sendFileOverSocket(String filePath, String host, int port) async {
  var completer = Completer<void>();

  // 파일에서 데이터 읽기
  var file = File(filePath);
  if (await file.exists()) {
    var fileBytes = await file.readAsBytes();

    // 소켓 연결 생성
    var socket = await Socket.connect(host, port);
    print('Connected to: ${socket.remoteAddress.address}:${socket.remotePort}');

    // 파일 데이터 전송
    socket.add(fileBytes);
    await socket.flush(); // 데이터 전송이 완료될 때까지 기다립니다.

    print('File sent successfully');
    socket.listen((data) {
      var response = utf8.decode(data);
      print('서버로부터 응답을 받았습니다: $response');
      socket.close();
    }, onDone: () {
      print('서버와의 연결이 종료되었습니다.');
    });

    completer.complete(); // 파일 전송 작업이 완료되었음을 알림
  } else {
    print('File does not exist.');
    completer.completeError('File not found');
  }

  return completer.future; // 이 Future를 반환하여 작업의 완료를 기다림
}

void main() async {
  var filePath = './data/maroon5';
  var host = '10.50.33.204';
  var port = 8081; // 예시 포트 번호

  await sendFileOverSocket(filePath, host, port);
  // exit(0); // 모든 작업이 완료된 후 프로그램 종료
}
