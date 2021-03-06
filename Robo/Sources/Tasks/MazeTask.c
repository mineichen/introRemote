#include "Tasks/MazeTask.h"
#include "Tasks/LedTask.h"



#define MAZE_DEFAUL_SPEED 1000

typedef enum {BACK, RIGHT, FORWARD, LEFT} direction;

static void turn(direction dir) {
	switch(dir) {
		case BACK:
			TURN_Turn(TURN_LEFT180, 0);
			break;
		case LEFT:
			TURN_Turn(TURN_LEFT90, 0);
			break;
		case RIGHT:
			TURN_Turn(TURN_RIGHT90, 0);
			break;
		case FORWARD:
			break;
		default:
			for(;;) {} // Unknown direction
	}
}

// Find End and return true when end is reached
_Bool findPath(MazeTask_t* that) {
   REF_LineKind lineKind;
   REF_LineKind stepLineKind;
   int pos = BACK;
   int foundSolution = 0;

   REF_SubscribeLineKindChange(that->lineKindChange);
   LF_StartFollowing();
   FRTOS1_xQueueReceive(that->lineKindChange, (void*) &lineKind, portMAX_DELAY);
   LF_StopFollowing();
   REF_UnsubscribeLineKindChange(that->lineKindChange);

   if (lineKind != REF_LINE_NONE) {
	   TURN_Turn(TURN_STEP_LINE_FW_POST_LINE, 0);
	   stepLineKind = REF_GetLineKind();

	   if(stepLineKind == REF_LINE_FULL) {
		   //Finished
		   foundSolution = 1;
	   }

	   if (!foundSolution && stepLineKind == REF_LINE_STRAIGHT) {
		   turn((4+FORWARD - pos ) % 4);
		   if(findPath(that)) foundSolution = 1;
		   pos = FORWARD;
	   }
	   if (!foundSolution && lineKind == REF_LINE_RIGHT || lineKind == REF_LINE_FULL) {
		   TURN_Turn(TURN_HALF_STEP_LINE_FW, 0);
		   turn((4+RIGHT - pos ) % 4);
		   if(findPath(that)) foundSolution = 1;
		   pos = RIGHT;
	   }
	   if(!foundSolution && lineKind == REF_LINE_LEFT || lineKind == REF_LINE_FULL) {
		   TURN_Turn(TURN_HALF_STEP_LINE_FW, 0);
		   turn((4+LEFT - pos ) % 4);
		   if(findPath(that)) foundSolution = 1;
		   pos = LEFT;
	   }
   }

   turn((4 - pos) % 4);
   REF_SubscribeLineKindChange(that->lineKindChange);
   LF_StartFollowing();
   FRTOS1_xQueueReceive(that->lineKindChange, (void*) &lineKind, portMAX_DELAY);
   LF_StopFollowing();
   REF_UnsubscribeLineKindChange(that->lineKindChange);

   TURN_Turn(TURN_STEP_LINE_FW_POST_LINE, 0);

   return false;
}


static void MazeTaskRun(void* pvParameters) {
	MazeTask_t* that = pvParameters;

		// Both direction
	if(findPath(that) || findPath(that)) {
	    //Biep("finishSong");
	}
	//Biep(noExit);
	/*
	for(;;) {

		vTaskDelay(1000/portTICK_PERIOD_MS);
	}*/
}


static void MazeTaskInit(void * params)
{

}
static void MazeTaskDeinit()
{
	FRTOS1_vQueueUnregisterQueue(MazeQueue);
}

MazeTask_t MazeTaskCreate()
{
	return (MazeTask_t) {
		{MazeTaskInit,MazeTaskRun,MazeTaskDeinit},
		FRTOS1_xQueueCreate(1000, sizeof(REF_LineKind))
	};
}

static void MAZE_PrintStatus(const CLS1_StdIOType *io) {
  CLS1_SendStatusStr((unsigned char*)"maze", (unsigned char*)"\r\n", io->stdOut);
  //CLS1_SendStatusStr((unsigned char*)"  solved", MAZE_IsSolved()?(unsigned char*)"yes\r\n":(unsigned char*)"no\r\n", io->stdOut);
  CLS1_SendStatusStr((unsigned char*)"  path", (unsigned char*)"(", io->stdOut);
  //CLS1_SendNum8u(pathLength, io->stdOut);
  CLS1_SendStr((unsigned char*)") ", io->stdOut);
  //for(int i=0;i<pathLength;i++) {
    //CLS1_SendStr(TURN_TurnKindStr(path[i]), io->stdOut);
    //CLS1_SendStr((unsigned char*)" ", io->stdOut);
  //}
  CLS1_SendStr((unsigned char*)"\r\n", io->stdOut);
}

static void MAZE_PrintHelp(const CLS1_StdIOType *io) {
  CLS1_SendHelpStr((unsigned char*)"maze", (unsigned char*)"Group of maze following commands\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  help|status", (unsigned char*)"Shows maze help or status\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  clear", (unsigned char*)"Clear the maze solution\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  start", (unsigned char*)"Clear the maze solution\r\n", io->stdOut);
}

uint8_t MAZE_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io) {
  uint8_t res = ERR_OK;

  if (UTIL1_strcmp((char*)cmd, (char*)CLS1_CMD_HELP)==0 || UTIL1_strcmp((char*)cmd, (char*)"maze help")==0) {
    MAZE_PrintHelp(io);
    *handled = TRUE;
  } else if (UTIL1_strcmp((char*)cmd, (char*)CLS1_CMD_STATUS)==0 || UTIL1_strcmp((char*)cmd, (char*)"maze status")==0) {
    MAZE_PrintStatus(io);
    *handled = TRUE;
  } else if (UTIL1_strcmp((char*)cmd, (char*)"maze start")==0) {
    //MAZE_ClearSolution();

	  startMaze();

    *handled = TRUE;
  }
  return res;
}

void startMaze(){
	static MazeTask_t mazeTask;
		mazeTask = MazeTaskCreate();
		IntroTask_t* mazeRawTask = (IntroTask_t*) &mazeTask;

		xTaskCreate(ApplicationRunTaskFn, "MazeTask", configMINIMAL_STACK_SIZE, mazeRawTask, tskIDLE_PRIORITY, mazeRawTask->taskHandle);

}


/*
int function findPath() {
  driveSteps(20);
  bool hasLeft;
  bool hasRight;
  int pos = 0;
  while(!(hasLeft = sensorHasLeft()) | !(hasRight = sensorHasRight())) {
     followLine();
  }

  driveSteps(DISTANCE_TILL_CROSS_IS_IN_CENTER_OF_ROBOT);
  if(sensorHasLeft() && sensorHasRight()) {
     //Finished
     return true;
   }

 if(sensorHasStraight()) {
      if(findPath() return true;
      int pos = STRAIGHT;
  }
 if(hasRight) {
    turn((4+RIGHT - pos ) % 4);
    if(findPath()) return true;
    int pos = RIGHT;
 }
 if(hasLeft()) {
   turn((4+LEFT - pos ) % 4);
   if(findPath()) return true;
   int pos = LEFT;
 }
turn(4 - pos);
return false;
}

*/
