#include "network.h"

struct NetworkInfo
{
    int ServerIDs;
    char IP[100];
    char port[20];
};


struct RoutingTable
{
    int SourceId;
    int NId;
    char NIP[100];
    char NPort[20];
    int IntermediateNode;
    int IsNeighbour;
    int cost;
    int IsDisabled;
    int Existed;
};

struct Display
{
    int Destination;
    int IntermNode;
    int Cost;
};

char *UpdateMessageMethod(char *ServerPort, char *ServerIPAdd, char *ServerIPAdd1, char *ServerPort1, char *ServerID1, char* ServerCost1);
char *RoutingMessage(char *ServerIP, char *ServerPort, struct RoutingTable *R);

void main(int argc, char*argv[])
{
    char startCommand[500];
    int SocketActivity;
    char ReadStandardInput[1000];

    struct RoutingTable RT[5];
    int RoutingTableIndex = 0;

    struct NetworkInfo NI[5];
    int NetworkInfoIndex = 0;


    //Display Command
    struct Display D[5];
    struct Display DisplayTemp[5];
    int DisplayIndex1;
    int DisplayIndex2;
    DisplayIndex1 = 0;
    DisplayIndex2 = 0;

    //Server Receiving Address
    struct sockaddr_in RecServer;
    struct sockaddr_in ToAddress;
    struct sockaddr_in FromAddress;
    int ToAddressLen;
    int FromAddressLen;
    struct timeval tv;

    char SendMSG[3000];
    int SendMSGLen;
    int validCMD;
    validCMD = 0;

    //Received message variables
    char RecMSG[3000];
    int RecMSGLen;
    char RecMSGTemp[3000];
    int index;
    index = 0;

    char stdMSG[1000];
    char stdMSGtemp[1000];
    char *RecStdMSG1Pointer;
    char RecStdMSG1[15];
    char *RecStdMSG2Pointer;
    char RecStdMSG2[15];
    char *RecStdMSG3Pointer;
    char RecStdMSG3[15];
    char *RecStdMSG4Pointer;
    char RecStdMSG4[15];

    int UpdateIDServer;
    int UpdateIDNeighbour;
    int UpdateCost;
    int UpdateValidServerFlag;
    int UpdateValidNeighboourFlag;
    int UpdateIsExistNeighbour;
    int UpdateIsDisabled;
    char UpdateNIP[50];
    char UpdateNPort[10];
    char *UpdateMSGSendPointer;
    char UpdateMSGSend[1000];
    char InfCost[]="99";

    //Receive Message Variables
    char **MSGElementArray;
    int MSGElementArrayCount;
    int InitialCost;
    int FinalCost;
    int MSGSenderID;
    int NumberUpdateFields;
    int UpdateIndexStart;
    int UpdateIndexStartTemp1;
    int UpdateIndexStartTemp2;
    int UpdateIndexStartTemp3;
    int EntryExistFlag;
    int MutualLinkFlag;

    //Step Command Variables
    char StepIP[50];
    char StepPort[10];
    char *RoutingMSGPointer;
    char RMSG[3000];

    //Periodic Updates
    char PeriodicIP[50];
    char PeriodicPort[10];
    char *PeriodicMSGPointer;
    char PeriodicMSG[3000];

    //Packet command variables
    int Packets;
    Packets = 0;

    //disable command variables
    char *DisableServerIDPointer;
    char DisableServerID[10];
    int DisableID;
    int DisableIDCheckFlag;
    DisableIDCheckFlag = 0;
    char DisableServerIP[50];
    char DisableServerPort[10];
    char *DisableMSGPointer;
    char DisableMSG[1000];
    char DisableCost[] = "100";
    char **DisableElementArray;
    int DisableIndex;
    DisableIndex = 0;



    //Creating read file descriptors
    fd_set readfds;

    //Nullifying the values in the structures
    for(RoutingTableIndex =0;RoutingTableIndex<5;RoutingTableIndex++)
    {
        RT[RoutingTableIndex].SourceId = 0;
        RT[RoutingTableIndex].NId = 0;
        strcpy(RT[RoutingTableIndex].NIP," ");
        strcpy(RT[RoutingTableIndex].NPort," ");
        RT[RoutingTableIndex].IntermediateNode =0;
        RT[RoutingTableIndex].IsNeighbour = 0;
        RT[RoutingTableIndex].cost = 0;
        RT[RoutingTableIndex].IsDisabled = 0;
        RT[RoutingTableIndex].Existed = 0;
    }


    for(NetworkInfoIndex=0;NetworkInfoIndex<5;NetworkInfoIndex++)
    {
        NI[NetworkInfoIndex].ServerIDs = 0;
        strcpy(NI[NetworkInfoIndex].port," ");
        strcpy(NI[NetworkInfoIndex].IP," ");
    }


    //getting the details of start Up from user
    int countStartCmdParametersFlag;
    countStartCmdParametersFlag = 0;

    if(argc != 5)
    {
        printf("%s : Incorrect Command  \n",startCommand);
        puts("-t <topology-file-name> -i <routing-update-interval>");
    }

    else
    {
    //Constructing Start Command
    strcpy(startCommand,"server");
    strcat(startCommand," ");
    strcat(startCommand,argv[1]);
    strcat(startCommand," ");
    strcat(startCommand,argv[2]);
    strcat(startCommand," ");
    strcat(startCommand,argv[3]);
    strcat(startCommand," ");
    strcat(startCommand,argv[4]);

    //validating Start Command
    countStartCmdParametersFlag = validateStartCommand(startCommand);
    if(countStartCmdParametersFlag != 1)
    {
        printf("%s : Incorrect Command  \n",startCommand);
        puts("-t <topology-file-name> -i <routing-update-interval>");
    }
    else
    {
        //puts("Correct Command");

        //Getting the file name
        char *FileNamePointer;
        char FileName[100];
        FileNamePointer = strSeperate(startCommand,3);
        strcpy(FileName,FileNamePointer);

        //Reading the file
        char *FileReadStringPointer;
        char FileReadString[1000];
        FileReadStringPointer =  FileString(FileName);
        strcpy(FileReadString,FileReadStringPointer);
        int fileSuccessFlag;
        fileSuccessFlag = 0;
        if(strcmp(FileReadString," ") != 0)
        {
            fileSuccessFlag=1;
        }
        else
        {
            puts("Error Reading File");
            fileSuccessFlag = 0;
        }

        if(fileSuccessFlag == 1)
        {

        //Breaking the file string into Array of Strings
            char **FileElementsArray;
            int FileElementsArrayIndex;
            int FileElementsArrayCount;
            int FileElementsArrayEndIndex;
            FileElementsArrayCount = 0;

            FileElementsArray = ArrayStrings(FileReadString);
            if (!FileElementsArray) {
                printf ("ERROR: Unable to allocate FileElementsArray!\n");
            }

            else {
                for(FileElementsArrayIndex=0;FileElementsArrayIndex<NUMBER;FileElementsArrayIndex++)
                {
                    if(strcmp(FileElementsArray[FileElementsArrayIndex]," ") == 0)
                    {

                    }
                    else
                    {
                        ++FileElementsArrayCount;
                        //printf("\n %d = ",FileElementsArrayIndex);
                        //puts(FileElementsArray[FileElementsArrayIndex]);
                    }
                }
            }

            //printf("%d \n ",FileElementsArrayCount);

            //Storing Data From File into Routing table
            FileElementsArrayEndIndex = (FileElementsArrayCount-1);
            int RoutingInformationStart;
            RoutingInformationStart = (FileElementsArrayEndIndex - (atoi(FileElementsArray[1])*3)+1);

            int j;
            int k;
            int tempVarRoutingInfo;

            for(j=RoutingInformationStart;j<FileElementsArrayCount;j=j+3)
            {
                for(k=0;k<5;k++)
                {
                    if(RT[k].SourceId == 0)
                    {
                        tempVarRoutingInfo = j;
                        RT[k].SourceId = atoi(FileElementsArray[tempVarRoutingInfo]);
                        RT[k].NId = atoi(FileElementsArray[tempVarRoutingInfo+1]);
                        RT[k].cost = atoi(FileElementsArray[tempVarRoutingInfo+2]);
                        RT[k].IsNeighbour = 1;
                        RT[k].IntermediateNode = RT[k].NId;
                        RT[k].IsDisabled = 0;
                        RT[k].Existed = 1;
                        //printf("j : %d \n",j);
                        break;
                    }
                }
            }


            //Storing Data into the Network Information Structure
            int NetworkInfoEndIndex;
            NetworkInfoEndIndex = (RoutingInformationStart-1);
            int m;
            int n;
            int tempVarNetworkInfo;
            for(m=2;m<NetworkInfoEndIndex;m=m+3)
            {
                for(n=0;n<5;n++)
                {
                    if(NI[n].ServerIDs == 0)
                    {
                        tempVarNetworkInfo = m;
                        NI[n].ServerIDs = atoi(FileElementsArray[tempVarNetworkInfo]);
                        strcpy(NI[n].IP,FileElementsArray[tempVarNetworkInfo+1]);
                        strcpy(NI[n].port,(FileElementsArray[tempVarNetworkInfo+2]));
                        //printf("m : %d \n",m);
                        break;
                    }
                }
            }


            for(k=0;k<5;k++)
            {
                for(n=0;n<5;n++)
                {
                    if(RT[k].NId == NI[n].ServerIDs)
                    {
                        strcpy(RT[k].NIP,NI[n].IP);
                        strcpy(RT[k].NPort,NI[n].port);
                    }
                }
            }

        char ServerIP[100];
        char ServerPort[10];
        for(n=0;n<5;n++)
        {
            if(RT[0].SourceId == NI[n].ServerIDs)
            {
                strcpy(ServerIP,NI[n].IP);
                strcpy(ServerPort,NI[n].port);
                break;
            }
        }

        //Creating Receiver Socket
        int RecSock;
        RecSock = socket(AF_INET , SOCK_DGRAM , 0);
        if(RecSock < 0)
        {
            puts("Error Creating RecSock");
        }

        //Creating Server Receiver Address
        RecServer.sin_family = AF_INET;
        inet_pton(AF_INET, ServerIP, &(RecServer.sin_addr));
        RecServer.sin_port = htons(atoi(ServerPort));
        bzero(RecServer.sin_zero,8);

        //Binding the socket to Server
        int bindVar;
        bindVar = bind(RecSock, (struct sockaddr *)&RecServer, sizeof(RecServer));
        if(bindVar < 0)
        {
            puts("Error Binding");
        }

        //Creating Server Sending Socket
        int SendSoc;
        SendSoc = socket(AF_INET , SOCK_DGRAM , 0);
        if(SendSoc<0)
        {
            puts("Error creating Sending Socket");
        }

        //creating file descriptor for Standard input
        int STDIN;
        STDIN = 0;

        int MaxSelect;
        MaxSelect = 0;

        //Getting the time interval
        char *timeSecondsPointer;
        char timeSecondsRec[10];
        timeSecondsPointer = strSeperate(startCommand,5);
        strcpy(timeSecondsRec,timeSecondsPointer);
        int timeSeconds;
        timeSeconds = atoi(timeSecondsRec);

        int SelectCount;
        SelectCount = 0;
        FromAddressLen = sizeof(struct sockaddr_in);
        //Starting While
        while(1)
        {
            UpdateIsExistNeighbour = 0;
            UpdateValidNeighboourFlag = 0;
            UpdateValidServerFlag = 0;
            UpdateIsDisabled = 0;

            SelectCount = 0;
            tv.tv_sec = timeSeconds;
            tv.tv_usec = 0;
            FD_ZERO(&readfds);
            FD_SET(STDIN, &readfds);
            FD_SET(RecSock, &readfds);
            if(RecSock>STDIN)
            {
                MaxSelect = RecSock;
            }
            else
            {
                MaxSelect = STDIN;
            }

            //checking the socket descriptors for activity
            SocketActivity = select(MaxSelect+1, &readfds, NULL, NULL, &tv);
            if ((SocketActivity < 0) && (errno!=EINTR))
            {
                puts("select Error...");
            }




            //checking the STDIN socket for activity
            if (FD_ISSET(STDIN, &readfds))
            {
                ++SelectCount;
                gets(stdMSG);
                validCMD = 0;


                strcpy(stdMSGtemp,stdMSG);
                RecStdMSG1Pointer = strSeperate(stdMSGtemp,1);
                strcpy(RecStdMSG1,RecStdMSG1Pointer);

                //checking the update link
                if(strcasecmp(RecStdMSG1,"update") == 0)
                   {
                       validCMD = 1;
                        RecStdMSG2Pointer = strSeperate(stdMSGtemp,4);
                        strcpy(RecStdMSG2,RecStdMSG2Pointer);
                        if(strcasecmp(RecStdMSG2,"inf") != 0)
                        {
                            RecStdMSG3Pointer = strSeperate(stdMSGtemp,2);
                            strcpy(RecStdMSG3, RecStdMSG3Pointer);
                            RecStdMSG4Pointer = strSeperate(stdMSGtemp,3);
                            strcpy(RecStdMSG4, RecStdMSG4Pointer);
                            //Checking if the values are correct
                            UpdateIDServer = atoi(RecStdMSG3);
                            UpdateIDNeighbour = atoi(RecStdMSG4);
                            UpdateCost = atoi(RecStdMSG2);
                            if(UpdateIDServer != RT[0].SourceId)
                            {
                                printf("%s : Incorrect Source ID \n", stdMSG);
                                UpdateValidServerFlag = 1;
                            }
                            for(k=0;k<5;k++)
                            {
                                if(UpdateIDNeighbour == RT[k].NId)
                                {
                                    UpdateIsExistNeighbour = 1;
                                }
                            }
                            if(UpdateIsExistNeighbour == 1)
                            {
                                for(k=0;k<5;k++)
                                {
                                    if(UpdateIDNeighbour == RT[k].NId)
                                    {
                                        if(RT[k].IsNeighbour == 1)
                                        {
                                            UpdateValidNeighboourFlag = 0;
                                        }
                                        else
                                        {
                                            if(RT[k].Existed == 1)
                                            {
                                                UpdateValidNeighboourFlag = 0;
                                            }
                                            else
                                            {
                                                printf(" %s : Not a neighbourID \n",RecMSG);
                                                UpdateValidNeighboourFlag = 1;
                                            }

                                        }
                                    }
                                }
                            }
                            if(UpdateIsExistNeighbour == 1)
                            {
                                for(k=0;k<5;k++)
                                {
                                    if(UpdateIDNeighbour == RT[k].NId)
                                    {
                                        if(RT[k].IsDisabled == 1)
                                        {
                                            UpdateIsDisabled = 1;
                                            printf(" %s : Node is disabled \n", stdMSG);
                                        }
                                        else
                                        {
                                            UpdateIsDisabled = 0;
                                        }
                                    }
                                }
                            }

                            if(UpdateIsExistNeighbour == 0)
                            {
                                printf(" %s : Invalid Neighbour ID \n", stdMSG);
                            }

                            //Performing Actions on correct values
                            if(UpdateIsExistNeighbour == 1)
                            {
                                if(UpdateValidServerFlag == 0)
                                {
                                    if(UpdateValidNeighboourFlag == 0)
                                    {
                                        if(UpdateIsDisabled == 0)
                                        {
                                        //Updating The Self Table
                                            for(k=0;k<5;k++)
                                            {
                                                if(RT[k].NId == UpdateIDNeighbour)
                                                {
                                                    RT[k].cost = UpdateCost;
                                                    RT[k].IntermediateNode = RT[k].NId;
                                                    RT[k].IsNeighbour = 1;
                                                    printf(" %s : SUCCESS", stdMSG);
                                                }
                                            }

                                        //Creating a send String
                                        for(k=0;k<5;k++)
                                        {
                                            if(UpdateIDNeighbour == RT[k].NId)
                                            {
                                                strcpy(UpdateNIP,RT[k].NIP);
                                                strcpy(UpdateNPort,RT[k].NPort);
                                                break;
                                            }
                                        }
                                        UpdateMSGSendPointer = UpdateMessageMethod(ServerPort,ServerIP,UpdateNIP,UpdateNPort,RecStdMSG4,RecStdMSG2);
                                        strcpy(UpdateMSGSend,UpdateMSGSendPointer);

                                        //Sending the Message
                                        ToAddress.sin_family = AF_INET;
                                        inet_pton(AF_INET, UpdateNIP, &(ToAddress.sin_addr));
                                        ToAddress.sin_port = htons(atoi(UpdateNPort));
                                        bzero(ToAddress.sin_zero,8);
                                        ToAddressLen = sizeof(struct sockaddr_in);
                                        strcpy(SendMSG,UpdateMSGSend);

                                        SendMSGLen = sendto(SendSoc,SendMSG,3000,0,(struct sockaddr *)&ToAddress,ToAddressLen);
                                        if(SendMSGLen<0)
                                        {
                                            puts("Error Sending Update");
                                        }
                                        else
                                        {
                                            //puts("MSG Sent");
                                        }
                                    }
                                    }
                                }
                            }
                        }

                   }//End of Update without inf


                //Starting Update INFYNITY
                /*if(strcasecmp(RecStdMSG1,"update") == 0)
                    {
                        RecStdMSG2Pointer = strSeperate(stdMSGtemp,4);
                        strcpy(RecStdMSG2,RecStdMSG2Pointer);
                        if(strcasecmp(RecStdMSG2,"inf") == 0)
                        {
                            RecStdMSG3Pointer = strSeperate(stdMSGtemp,2);
                            strcpy(RecStdMSG3, RecStdMSG3Pointer);
                            RecStdMSG4Pointer = strSeperate(stdMSGtemp,3);
                            strcpy(RecStdMSG4, RecStdMSG4Pointer);
                            //Checking if the values are correct
                            UpdateIDServer = atoi(RecStdMSG3);
                            UpdateIDNeighbour = atoi(RecStdMSG4);
                            UpdateCost = 99;
                            if(UpdateIDServer != RT[0].SourceId)
                            {
                                puts("Incorrect Server ID");
                                UpdateValidServerFlag = 1;
                            }
                            for(k=0;k<5;k++)
                            {
                                if(UpdateIDNeighbour == RT[k].NId)
                                {
                                    UpdateIsExistNeighbour = 1;
                                }
                            }
                            if(UpdateIsExistNeighbour == 1)
                            {
                                for(k=0;k<5;k++)
                                {
                                    if(UpdateIDNeighbour == RT[k].NId)
                                    {
                                        if(RT[k].IsNeighbour == 1)
                                        {
                                            UpdateValidNeighboourFlag = 0;
                                        }
                                        else
                                        {
                                            puts("Not a neighbour or already infinity");
                                            UpdateValidNeighboourFlag = 1;
                                        }

                                    }
                                }
                            }
                            //}
                            if(UpdateIsExistNeighbour == 1)
                            {
                                for(k=0;k<5;k++)
                                {
                                    if(UpdateIDNeighbour == RT[k].NId)
                                    {
                                        if(RT[k].IsDisabled == 1)
                                        {
                                            UpdateIsDisabled = 1;
                                            puts("Node is disabled");
                                        }
                                        else
                                        {
                                            UpdateIsDisabled = 0;
                                        }
                                    }
                                }
                            }

                            if(UpdateIsExistNeighbour == 0)
                            {
                                puts("Invalid Neighbour Info");
                            }

                            if(UpdateIsExistNeighbour == 1)
                            {
                                if(UpdateValidServerFlag == 0)
                                {
                                    if(UpdateValidNeighboourFlag == 0)
                                    {
                                        if(UpdateIsDisabled == 0)
                                        {
                                            //Updating the self Table
                                            for(k=0;k<5;k++)
                                            {
                                                if(RT[k].NId == UpdateIDNeighbour)
                                                {
                                                    RT[k].cost = UpdateCost;
                                                    RT[k].IntermediateNode = 0;
                                                    RT[k].IsNeighbour = 0;
                                                    puts("Updated Successfully");
                                                }
                                            }


                                            //Creating a send String
                                            for(k=0;k<5;k++)
                                            {
                                                if(UpdateIDNeighbour == RT[k].NId)
                                                {
                                                    strcpy(UpdateNIP,RT[k].NIP);
                                                    strcpy(UpdateNPort,RT[k].NPort);
                                                    break;
                                                }
                                            }

                                            UpdateMSGSendPointer = UpdateMessageMethod(ServerPort,ServerIP,UpdateNIP,UpdateNPort,RecStdMSG4,InfCost);
                                            strcpy(UpdateMSGSend,UpdateMSGSendPointer);

                                            //Sending the Message
                                            ToAddress.sin_family = AF_INET;
                                            inet_pton(AF_INET, UpdateNIP, &(ToAddress.sin_addr));
                                            ToAddress.sin_port = htons(atoi(UpdateNPort));
                                            bzero(ToAddress.sin_zero,8);
                                            ToAddressLen = sizeof(struct sockaddr_in);
                                            strcpy(SendMSG,UpdateMSGSend);

                                            SendMSGLen = sendto(SendSoc,SendMSG,3000,0,(struct sockaddr *)&ToAddress,ToAddressLen);
                                            if(SendMSGLen<0)
                                            {
                                                puts("Error Sending MSG");
                                            }
                                            else
                                            {
                                                puts("MSG Sent");
                                            }

                                        }

                                    }

                                }
                            }
                        }//End of String Compare to inf
                }*/
                //End of Update Inf


                if(strcasecmp(RecStdMSG1,"Display") == 0)
                {
                    validCMD = 1;
                    for(k=0;k<5;k++)
                    {
                        D[k].Destination = RT[k].NId;
                        D[k].IntermNode = RT[k].IntermediateNode;
                        D[k].Cost = RT[k].cost;
                    }

                    DisplayTemp[0].Destination = D[0].Destination;
                    DisplayTemp[0].IntermNode = D[0].IntermNode;
                    DisplayTemp[0].Cost = D[0].Cost;

                    for(DisplayIndex1 = 0;DisplayIndex1<5;DisplayIndex1++)
                    {
                        for(DisplayIndex2 = DisplayIndex1+1;DisplayIndex2<5;DisplayIndex2++)
                        {
                            if( D[DisplayIndex1].Destination > D[DisplayIndex2].Destination)
                            {
                                DisplayTemp[0].Destination = D[DisplayIndex1].Destination;
                                DisplayTemp[0].IntermNode = D[DisplayIndex1].IntermNode;
                                DisplayTemp[0].Cost = D[DisplayIndex1].Cost;

                                D[DisplayIndex1].Destination = D[DisplayIndex2].Destination;
                                D[DisplayIndex1].IntermNode = D[DisplayIndex2].IntermNode;
                                D[DisplayIndex1].Cost = D[DisplayIndex2].Cost;

                                D[DisplayIndex2].Destination = DisplayTemp[0].Destination;
                                D[DisplayIndex2].IntermNode = DisplayTemp[0].IntermNode;
                                D[DisplayIndex2].Cost = DisplayTemp[0].Cost;
                            }
                        }
                    }

                    puts("destination-server-ID     next-hop-server-ID     cost-of-path");
                    for(k=0;k<5;k++)
                    {
                        if( D[k].Destination !=0 )
                        {
                            printf("  %d                         %d                        %d \n", D[k].Destination,D[k].IntermNode, D[k].Cost );
                        }
                    }
                }//End of Display


                //begin step command
                if(strcasecmp(RecStdMSG1,"Step") ==  0)
                {
                    validCMD = 1;
                    RoutingMSGPointer = RoutingMessage(ServerIP,ServerPort,RT);
                    strcpy(RMSG,RoutingMSGPointer);


                    for(k=0;k<5;k++)
                    {
                        if(RT[k].IsNeighbour == 1)
                        {
                            strcpy(StepIP,RT[k].NIP);
                            strcpy(StepPort,RT[k].NPort);
                            ToAddress.sin_family = AF_INET;
                            inet_pton(AF_INET, StepIP, &(ToAddress.sin_addr));
                            ToAddress.sin_port = htons(atoi(StepPort));
                            bzero(ToAddress.sin_zero,8);
                            ToAddressLen = sizeof(struct sockaddr_in);

                            strcpy(SendMSG,RMSG);
                            SendMSGLen = sendto(SendSoc,SendMSG,3000,0,(struct sockaddr *)&ToAddress,ToAddressLen);
                            if(SendMSGLen<0)
                            {
                                //puts("Error Sending MSG");
                            }
                            else
                            {
                                puts("Step : SUCCESS");
                            }

                        }
                    }
                }//End of step command


                //Begin packets command
                if(strcasecmp(RecStdMSG1,"packets") == 0)
                {
                    validCMD = 1;
                    printf("packets SUCCESS: %d \n", Packets);
                    Packets = 0;
                }//End of packets command


                //Starting disable command
                /*if(strcasecmp(RecStdMSG1,"disable") == 0)
                {
                    DisableIDCheckFlag = 0;
                    DisableServerIDPointer = strSeperate(stdMSGtemp,2);
                    strcpy(DisableServerID,DisableServerIDPointer);
                    DisableID = atoi(DisableServerID);
                    for(k=0;k<5;k++)
                    {
                        if(RT[k].NId == DisableID)
                        {
                            if(RT[k].IsNeighbour == 1)
                            {
                                DisableIDCheckFlag = 1;
                                break;
                            }
                        }
                    }

                    if(DisableIDCheckFlag == 1)
                    {
                        //Updating the self table
                        for(k=0;k<5;k++)
                        {
                            if(RT[k].NId == DisableID)
                            {
                                RT[k].cost = 99;
                                RT[k].IsNeighbour = 0;
                                RT[k].IntermediateNode = 0;
                                RT[k].IsDisabled = 1;
                            }
                        }

                        //obtaining the IP of the server and port
                        for(k=0;k<5;k++)
                        {
                            if(RT[k].NId == DisableID)
                            {
                                strcpy(DisableServerIP,RT[k].NIP);
                                strcpy(DisableServerPort,RT[k].NPort);
                                break;
                            }
                        }


                        //Sending the message to the other end
                        DisableMSGPointer = UpdateMessageMethod(ServerPort,ServerIP,DisableServerIP,DisableServerPort,DisableServerID,DisableCost);
                        strcpy(DisableMSG,DisableMSGPointer);

                        ToAddress.sin_family = AF_INET;
                        inet_pton(AF_INET, DisableServerIP, &(ToAddress.sin_addr));
                        ToAddress.sin_port = htons(atoi(DisableServerPort));
                        bzero(ToAddress.sin_zero,8);

                        ToAddressLen = sizeof(struct sockaddr_in);
                        strcpy(SendMSG,DisableMSG);
                        SendMSGLen = sendto(SendSoc,SendMSG,3000,0,(struct sockaddr *)&ToAddress,ToAddressLen);
                        if(SendMSGLen<0)
                            {
                                //puts("Error Sending MSG");
                            }
                            else
                            {
                                //puts("MSG Sent");
                            }

                    }//Ending if DisableCheckFlag == 1
                    else if(DisableIDCheckFlag == 0)
                    {
                        puts("DISABLED: server Already disabled or set to infinity");
                    }
                }*/
                //Ending disable command

                    if(validCMD == 0)
                    {
                        puts("NO Such Command");
                    }

            }//Ending STDIN Read_FD










            //Checking the Receiving Socket
            if(FD_ISSET(RecSock, &readfds))
            {
                ++SelectCount;
                memset(RecMSG, 0 , sizeof(RecMSG));
                RecMSGLen = recvfrom(RecSock,RecMSG,3000,0,(struct sockaddr *)&FromAddress,&FromAddressLen);
                if(RecMSGLen < 0)
                {
                    puts("ERROR: In Receiving!");
                }
                else
                {
                    RecMSG[strlen(RecMSG)] = '\0';
                    ++Packets;
                    strcpy(RecMSGTemp,RecMSG);
                    MSGElementArrayCount = 0;
                    MSGElementArray = ArrayStrings(RecMSGTemp);
                    if (!MSGElementArray) {
                    printf ("ERROR: In Receiving!\n");
                    }

                    else {
                        for(index=0;index<NUMBER;index++)
                        {
                            if(strcmp(MSGElementArray[index]," ") == 0)
                            {

                            }
                            else
                            {
                                ++MSGElementArrayCount;
                                //printf("\n %d = \n",index);
                                //puts(MSGElementArray[index]);
                            }
                        }
                    }

                    //If message length is not equal to 7
                    if(MSGElementArrayCount != 7)
                    {

                        for(k=0;k<5;k++)
                        {
                            if(strcmp(MSGElementArray[2],RT[k].NIP) == 0)
                            {
                                MSGSenderID = RT[k].NId;
                                printf("RECEIVED A MESSAGE FROM SERVER %d  \n", MSGSenderID);
                                break;
                            }
                        }

                        NumberUpdateFields = atoi(MSGElementArray[0]);
                        //Updating the link between two neighbours if that is less than the stored value
                        for(UpdateIndexStart = 5;UpdateIndexStart<MSGElementArrayCount;UpdateIndexStart=UpdateIndexStart+4)
                        {
                            UpdateIndexStartTemp1 = UpdateIndexStart;
                            UpdateIndexStartTemp2 = UpdateIndexStart;
                            if(atoi(MSGElementArray[UpdateIndexStartTemp1]) == RT[0].SourceId)
                            {
                                for(k=0;k<5;k++)
                                {
                                    if(RT[k].NId == MSGSenderID)
                                    {

                                        if(atoi(MSGElementArray[UpdateIndexStartTemp1+1])< RT[k].cost)
                                        {
                                            RT[k].cost = atoi(MSGElementArray[UpdateIndexStartTemp2+1]);
                                            RT[k].IntermediateNode = MSGSenderID;
                                            RT[k].IsDisabled = 0;
                                            RT[k].IsNeighbour = 1;
                                        }
                                        break;
                                    }
                                }
                            }
                        }

                        //Calculating the Initial cost for the mutual link
                        for(k=0;k<5;k++)
                        {
                            if(strcmp(MSGElementArray[2],RT[k].NIP) == 0)
                            {
                                InitialCost = RT[k].cost;
                                break;
                            }
                        }

                        //Starting for the Message again
                        for(UpdateIndexStart = 5;UpdateIndexStart<MSGElementArrayCount;UpdateIndexStart=UpdateIndexStart+4)
                        {
                            UpdateIndexStartTemp1 = UpdateIndexStart;
                            UpdateIndexStartTemp2 = UpdateIndexStart;
                            EntryExistFlag = 0;
                            MutualLinkFlag = 0;

                            for(n=0;n<5;n++)
                            {
                                if(atoi(MSGElementArray[UpdateIndexStartTemp1]) == RT[n].NId)
                                {
                                    EntryExistFlag = 1;
                                    break;
                                }
                            }

                            if(atoi(MSGElementArray[UpdateIndexStartTemp1]) == RT[0].SourceId)
                            {
                                EntryExistFlag = 1;
                            }


                            if(EntryExistFlag == 1)
                            {
                                for(k=0;k<5;k++)
                                {
                                    if(atoi(MSGElementArray[UpdateIndexStartTemp1]) == RT[k].NId)
                                    {
                                        if(atoi(MSGElementArray[UpdateIndexStartTemp2+1]) == 99)
                                        {
                                            RT[k].cost = 99;
                                            RT[k].IntermediateNode = 0;
                                            UpdateIndexStartTemp2 = UpdateIndexStart;
                                        }

                                        else{

                                        if(RT[k].IntermediateNode == MSGSenderID)
                                        {
                                            RT[k].cost = (InitialCost+atoi(MSGElementArray[UpdateIndexStartTemp1+1]));
                                            UpdateIndexStartTemp1 = UpdateIndexStart;
                                            RT[k].IntermediateNode = MSGSenderID;
                                        }

                                        if(RT[k].IntermediateNode != MSGSenderID)
                                        {
                                            FinalCost =(InitialCost+atoi(MSGElementArray[UpdateIndexStartTemp1+1]));
                                            if(FinalCost<RT[k].cost)
                                            {
                                                RT[k].cost = FinalCost;
                                                RT[k].IntermediateNode = MSGSenderID;
                                            }
                                            UpdateIndexStartTemp1 = UpdateIndexStart;
                                        }

                                        }



                                    }
                                }
                            }


                            if(EntryExistFlag == 0)
                            {
                                for(k=0;k<5;k++)
                                {
                                    if(RT[k].SourceId == 0)
                                    {
                                        RT[k].SourceId = atoi(FileElementsArray[RoutingInformationStart]);
                                        RT[k].IsNeighbour = 0;
                                        RT[k].NId = atoi(MSGElementArray[UpdateIndexStartTemp1]);
                                        RT[k].cost = InitialCost + atoi(MSGElementArray[UpdateIndexStartTemp1+1]);
                                        RT[k].IntermediateNode = MSGSenderID;
                                        UpdateIndexStartTemp1 = UpdateIndexStart;
                                        break;
                                    }
                                }
                            }
                        }//Ending starting for the message







                    }//End of if message is not equal to 7

                    //If message count is equal to 7
                    else if(MSGElementArrayCount == 7)
                    {

                        for(k=0;k<5;k++)
                        {
                            if(strcmp(MSGElementArray[2],RT[k].NIP) == 0)
                            {
                                MSGSenderID = RT[k].NId;
                                printf("RECEIVED A MESSAGE FROM SERVER %d  \n", MSGSenderID);
                                break;
                            }
                        }

                        NumberUpdateFields = atoi(MSGElementArray[0]);
                        for(UpdateIndexStart = 5;UpdateIndexStart<MSGElementArrayCount;UpdateIndexStart=UpdateIndexStart+4)
                        {
                            UpdateIndexStartTemp1 = UpdateIndexStart;
                            UpdateIndexStartTemp2 = UpdateIndexStart;
                            UpdateIndexStartTemp3 = UpdateIndexStart;
                            if(atoi(MSGElementArray[UpdateIndexStartTemp1]) == RT[0].SourceId)
                            {
                                for(k=0;k<5;k++)
                                {
                                    if(RT[k].NId == MSGSenderID)
                                    {

                                        if(atoi(MSGElementArray[UpdateIndexStartTemp1+1]) == 99)
                                        {
                                            RT[k].cost = atoi(MSGElementArray[6]);
                                            RT[k].IntermediateNode = 0;
                                            RT[k].IsNeighbour = 0;



                                        }

                                        if(atoi(MSGElementArray[UpdateIndexStartTemp3+1]) == 100)
                                        {
                                            RT[k].cost = 99;
                                            RT[k].IntermediateNode = 0;
                                            RT[k].IsNeighbour = 0;
                                            RT[k].IsDisabled = 1;

                                        }

                                        if(atoi(MSGElementArray[UpdateIndexStartTemp2+1]) < 99)
                                        {
                                            RT[k].cost = atoi(MSGElementArray[6]);
                                            RT[k].IntermediateNode = MSGSenderID;
                                            RT[k].IsNeighbour = 1;
                                            RT[k].IsDisabled = 0;

                                        }
                                        break;



                                    }
                                }
                            }
                        }

                    }//Ending Message for count 7



                }// Successful message read
            }// End of FD_ISSET receive message







            //For time out
            else
            {
                if(SelectCount == 0)
                {
                    //puts("Periodic update sent");
                    PeriodicMSGPointer = RoutingMessage(ServerIP,ServerPort,RT);
                    strcpy(PeriodicMSG,PeriodicMSGPointer);
                    for(k=0;k<5;k++)
                    {
                        if(RT[k].IsNeighbour == 1)
                        {
                            strcpy(PeriodicIP, RT[k].NIP);
                            strcpy(PeriodicPort, RT[k].NPort);
                            ToAddress.sin_family = AF_INET;
                            inet_pton(AF_INET, PeriodicIP, &(ToAddress.sin_addr));
                            ToAddress.sin_port = htons(atoi(PeriodicPort));
                            bzero(ToAddress.sin_zero,8);
                            ToAddressLen = sizeof(struct sockaddr_in);

                            strcpy(SendMSG, PeriodicMSG);
                            SendMSGLen = sendto(SendSoc,SendMSG,3000,0,(struct sockaddr *)&ToAddress,ToAddressLen);
                            if(SendMSGLen<0)
                            {
                                puts("Error Sending periodic MSG");
                            }
                            else
                            {
                                puts("Periodic Message sent");
                            }
                        }
                    }


                }

            }


















        }//Closing of While Loop
        }//Ending Correct File Read
    } //Ending Correct Server Start Command
    }
} //Ending Main






char *UpdateMessageMethod(char *ServerPort, char *ServerIPAdd, char *ServerIPAdd1, char *ServerPort1, char *ServerID1, char* ServerCost1)
{
    char ServerPortFinal[10];
    char ServerIPAddFinal[50];
    char ServerIPAdd1Final[50];
    char ServerPort1Final[10];
    char ServerID1Final[5];
    char ServerCost1Final[10];
    char updateField[] = "1";

    static char a[1000];

    strcpy(ServerPortFinal,ServerPort);
    strcpy(ServerIPAddFinal,ServerIPAdd);
    strcpy(ServerIPAdd1Final,ServerIPAdd1);
    strcpy(ServerPort1Final,ServerPort1);
    strcpy(ServerID1Final,ServerID1);
    strcpy(ServerCost1Final,ServerCost1);

    strcpy(a,updateField);
    strcat(a," ");
    strcat(a,ServerPortFinal);
    strcat(a," ");
    strcat(a,ServerIPAddFinal);
    strcat(a," ");
    strcat(a,ServerIPAdd1Final);
    strcat(a," ");
    strcat(a,ServerPort1Final);
    strcat(a," ");
    strcat(a,ServerID1Final);
    strcat(a," ");
    strcat(a,ServerCost1Final);

    return a;
}



char *RoutingMessage(char *ServerIP, char *ServerPort, struct RoutingTable *R)
{
    int i;
    i=0;
    int countUpdates;
    char *countUpdatesPointer;
    char countUpdatesChar[5];
    countUpdates = 0;
    char ServerIPFinal[50];
    char ServerPortFinal[15];
    static char RoutingMSG[3000];
    strcpy(ServerIPFinal,ServerIP);
    strcpy(ServerPortFinal,ServerPort);

    char *R0IDP;
    char *R0CostP;
    char R0ID[5];
    char R0Cost[5];

    char *R1IDP;
    char *R1CostP;
    char R1ID[5];
    char R1Cost[5];

    char *R4IDP;
    char *R4CostP;
    char R4ID[5];
    char R4Cost[5];

    char *R2IDP;
    char *R2CostP;
    char R2ID[5];
    char R2Cost[5];

    char *R3IDP;
    char *R3CostP;
    char R3ID[5];
    char R3Cost[5];


    for(i=0;i<5;i++)
    {
        if( R[i].SourceId !=0  )
        {
            ++countUpdates;
        }
    }
    //puts("inside function");
    countUpdatesPointer = IntToString(countUpdates);
    strcpy(countUpdatesChar,countUpdatesPointer);
    //puts("IntToString over");

    strcpy(RoutingMSG,countUpdatesChar);
    strcat(RoutingMSG," ");
    strcat(RoutingMSG,ServerPortFinal);
    strcat(RoutingMSG," ");
    strcat(RoutingMSG,ServerIPFinal);
    strcat(RoutingMSG," ");

    //puts("Above IF");
    if( R[0].SourceId != 0 )
    {
        strcat(RoutingMSG,"xxx");
        strcat(RoutingMSG," ");
        strcat(RoutingMSG,"xxx");
        strcat(RoutingMSG," ");
        R0IDP = IntToString(R[0].NId);
        strcpy(R0ID,R0IDP);
        R0CostP = IntToString(R[0].cost);
        strcpy(R0Cost,R0CostP);
        strcat(RoutingMSG,R0ID);
        strcat(RoutingMSG," ");
        strcat(RoutingMSG,R0Cost);
        strcat(RoutingMSG," ");
    }

    if( R[1].SourceId != 0 )
    {
        strcat(RoutingMSG,"xxx");
        strcat(RoutingMSG," ");
        strcat(RoutingMSG,"xxx");
        strcat(RoutingMSG," ");
        R1IDP = IntToString(R[1].NId);
        strcpy(R1ID,R1IDP);
        R1CostP = IntToString(R[1].cost);
        strcpy(R1Cost,R1CostP);
        strcat(RoutingMSG,R1ID);
        strcat(RoutingMSG," ");
        strcat(RoutingMSG,R1Cost);
        strcat(RoutingMSG," ");
    }

    if( R[2].SourceId != 0 )
    {
        strcat(RoutingMSG,"xxx");
        strcat(RoutingMSG," ");
        strcat(RoutingMSG,"xxx");
        strcat(RoutingMSG," ");
        R2IDP = IntToString(R[2].NId);
        strcpy(R2ID,R2IDP);
        R2CostP = IntToString(R[2].cost);
        strcpy(R2Cost,R2CostP);
        strcat(RoutingMSG,R2ID);
        strcat(RoutingMSG," ");
        strcat(RoutingMSG,R2Cost);
        strcat(RoutingMSG," ");
    }

    if( R[3].SourceId != 0 )
    {
        strcat(RoutingMSG,"xxx");
        strcat(RoutingMSG," ");
        strcat(RoutingMSG,"xxx");
        strcat(RoutingMSG," ");
        R3IDP = IntToString(R[3].NId);
        strcpy(R3ID,R3IDP);
        R3CostP = IntToString(R[3].cost);
        strcpy(R3Cost,R3CostP);
        strcat(RoutingMSG,R3ID);
        strcat(RoutingMSG," ");
        strcat(RoutingMSG,R3Cost);
        strcat(RoutingMSG," ");
    }

    if( R[4].SourceId != 0 )
    {
        strcat(RoutingMSG,"xxx");
        strcat(RoutingMSG," ");
        strcat(RoutingMSG,"xxx");
        strcat(RoutingMSG," ");
        R4IDP = IntToString(R[4].NId);
        strcpy(R4ID,R4IDP);
        R4CostP = IntToString(R[4].cost);
        strcpy(R4Cost,R4CostP);
        strcat(RoutingMSG,R4ID);
        strcat(RoutingMSG," ");
        strcat(RoutingMSG,R4Cost);
        strcat(RoutingMSG," ");
    }
    //puts("Above return");
    return RoutingMSG;
}

