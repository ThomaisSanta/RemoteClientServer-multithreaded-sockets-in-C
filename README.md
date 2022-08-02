### Client Server model, communication through sockets using multiple threads

## Usage
To execute the code of the current repository type make in the directory of the project after cloning. <br/>
Then to execute server's code run the following command in the terminal <br/>
**./dataServer -p <portNumber> -s <threadPoolSize> -q <QueueSize> -b <BlockSize> ** <br/>
where **portNumber** is the number of the port in which server listens to for new connections  <br/>
**threadPoolSize** is the number of the threads that will be made during this project <br/>
**QueueSize** is the size of the queue in which requests will be saved <br/>
and **BlockSize** is the size of <br/>
<br/> Then open a new terminal in the current project to execute client's code <br/>
use the following command: **./remoteClient -i <serverIp> -p <serverPort> -d <directory> ** <br/>

