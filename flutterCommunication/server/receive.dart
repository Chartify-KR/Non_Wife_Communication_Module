import 'dart:io';
import 'dart:async';
import 'dart:convert';

Future<void> startServer(int port) async {
  // 서버 소켓 생성
  var serverSocket = await ServerSocket.bind(InternetAddress.anyIPv4, port);
  print('서버가 ${serverSocket.address.address}:${serverSocket.port}에서 실행 중입니다.');

  await for (var client in serverSocket) {
    print(
        '클라이언트가 연결되었습니다: ${client.remoteAddress.address}:${client.remotePort}');

    // 파일 데이터를 저장할 파일 스트림 생성
    var file = File('./temp.txt').openWrite();

    // 클라이언트로부터 데이터 수신
    client.listen((data) {
      print('데이터 수신 중...');
      // 파일에 데이터 쓰기
      file.add(data);
      var message = utf8.decode(data);
      print('클라이언트로부터 메시지를 받았습니다: $message');
      client.write('메시지를 받았습니다: "$message"');
    }, onDone: () async {
      await fileReplacer("./data/cake.txt", "temp.txt");
      await file.close();
      print('파일 전송 완료 및 저장 완료');
      client.close();
    }, onError: (error) {
      print('데이터 수신 중 오류 발생: $error');
      client.close();
    });
  }
}

void main() async {
  var port = 8081; // 예시 포트 번호
  await startServer(port);
}

Future<void> fileReplacer(String originalFile, String newFile) async {
  // 오리지널 파일의 File 객체 생성
  var original = File(originalFile);

  // 새 파일의 File 객체 생성
  var replacement = File(newFile);

  // 오리지널 파일이 존재하는지 확인
  if (await original.exists()) {
    // 오리지널 파일 삭제
    await original.delete();
    print('Original file deleted.');
  } else {
    print('Original file does not exist.');
  }

  // 새 파일의 이름을 오리지널 파일의 이름으로 변경
  await replacement.rename(originalFile);
  print('New file renamed to original file name.');
}
