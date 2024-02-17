import 'dart:io';
import 'dart:convert';

Future<void> main() async {
  // 서버 생성
  var server = await ServerSocket.bind(InternetAddress.anyIPv4, 8081);
  print('서버가 ${server.address.address}:${server.port}에서 실행 중입니다.');

  await for (var client in server) {
    print(
        '클라이언트가 연결되었습니다: ${client.remoteAddress.address}:${client.remotePort}');
    client.listen((data) {
      var message = utf8.decode(data);
      print('클라이언트로부터 메시지를 받았습니다: $message');
      // 클라이언트에 응답 보내기
      client.write('메시지를 받았습니다: "$message"');
    }, onDone: () {
      print('클라이언트와의 연결이 종료되었습니다.');
      client.close();
    });
  }
}
