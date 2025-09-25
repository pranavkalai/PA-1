/*
	Original author of the starter code
    Tanzir Ahmed
    Department of Computer Science & Engineering
    Texas A&M University
    Date: 2/8/20
	
	Please include your Name, UIN, and the date below
	Name: Pranav Kalaiselvan
	UIN: 534000102
	Date: 09/18/2025
*/
#include "common.h"
#include "FIFORequestChannel.h"
#include <sys/wait.h>

using namespace std;


int main (int argc, char *argv[]) {
	int opt;
	int p = -1;
	double t = -1.0;
	int e = -1;
	int m_size = MAX_MESSAGE;

	string filename = "";
	while ((opt = getopt(argc, argv, "p:t:e:f:")) != -1) {
		switch (opt) {
			case 'p':
				p = atoi (optarg);
				break;
			case 't':
				t = atof (optarg);
				break;
			case 'e':
				e = atoi (optarg);
				break;
			case 'f':
				filename = optarg;
				break;
			case 'm':
				m_size = atoi (optarg);
				break;
		}
	}

	char m_str[16];
	sprintf(m_str, "%d", m_size);

	char* args[] = {(char*)"./server", (char*)"-m", m_str, NULL};
	
	pid_t pid = fork();
	if (pid < 0) {
		perror("Fork Failed");
		return 1;
	}
	else if (pid == 0) { // child
		// run server
		execvp(args[0], args);
	}
	else { // parent
		
		sleep(1); // wait for server to run in child process
		FIFORequestChannel chan("control", FIFORequestChannel::CLIENT_SIDE);
		char buf[MAX_MESSAGE]; // 256

		if (p != -1 && t != -1 && e != -1) { // p, t, e all specified
			// example data point request
			datamsg x(p, t, e);
			
			memcpy(buf, &x, sizeof(datamsg));
			chan.cwrite(buf, sizeof(datamsg)); // question to server
			double reply; // var to store answer from server
			chan.cread(&reply, sizeof(double)); // answer
			cout << "For person " << p << ", at time " << t << ", the value of ecg " << e << " is " << reply << endl;
		} 
		else if (p != -1) { // only p specified
			// make sure received dir exists 
			ofstream ofs("received/x1.csv");
			t = 0.0;
			for (int i = 0; i < 1000; i++) {
				datamsg x(p, t, 1);
			
				memcpy(buf, &x, sizeof(datamsg));
				chan.cwrite(buf, sizeof(datamsg)); // question to server
				double ecg1; // var to store ecg1 from server
				chan.cread(&ecg1, sizeof(double)); // answer

				datamsg y(p, t, 2);
			
				memcpy(buf, &y, sizeof(datamsg));
				chan.cwrite(buf, sizeof(datamsg)); // question to server
				double ecg2; // var to store ecg2 from server
				chan.cread(&ecg2, sizeof(double)); // answer

				ofs << t << "," << ecg1 << "," << ecg2 << endl;

				t += 0.004;
			}
			ofs.close();

			cout << "First 1000 data points for person " << p << " written to x1.csv" << endl;
		}
		// WORK ON THIS LATER

		// sending a non-sense message, you need to change this
		filemsg fm(0, 0);
		string fname = filename;
		
		int len = sizeof(filemsg) + (fname.size() + 1);
		char* buf2 = new char[len]; // buffer request
		memcpy(buf2, &fm, sizeof(filemsg));
		strcpy(buf2 + sizeof(filemsg), fname.c_str());
		chan.cwrite(buf2, len);  // I want the file length;
		
		//loops_needed = len / m_size + (len % m_size != 0);
		//for (int i = 0; i < loops_needed; i++) {
	
		//}

		int64_t file_size = 0;
		chan.cread(&file_size, sizeof(__int64_t));

		delete[] buf2;
		
		// closing the channel    
		MESSAGE_TYPE m = QUIT_MSG;
		chan.cwrite(&m, sizeof(MESSAGE_TYPE));
		//wait(NULL);

		}

    

}
