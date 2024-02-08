#include <iostream>
#include <cstring> // for memset
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main() {
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    char buffer[65]; // 64 바이트 메시지 + 널 종료 문자

    // 소켓 생성
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // 서버 정보 채우기
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY; // 모든 인터페이스에서 수신
    servaddr.sin_port = htons(8080); // 포트 8080에서 수신

    // 소켓에 서버 정보 바인딩
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    while (true) {
        unsigned int len = sizeof(cliaddr); // 클라이언트 주소의 길이

        // 데이터 수신
        int n = recvfrom(sockfd, buffer, 64, MSG_WAITALL, (struct sockaddr *)&cliaddr, &len);
        buffer[n] = '\0'; // 문자열 종료
        std::cout << "Received message: " << buffer << std::endl;
    }

    close(sockfd);
    return 0;
}
