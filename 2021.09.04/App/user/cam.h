#ifndef _CAM_H_
#define _CAM_H_

#include "headfile.h"

#define  		ROW 					60
#define  		COL 					128
#define 		CenterColumn 			64//������
#define 		LIMIT_COL(x,_max,_min)  (x > _max ? x = _max : (x < _min ? x = _min : x))

//#define
typedef struct
{
    uint8 x;
    uint8 y;
} PosiType;

typedef struct
{
    int16 Center[ROW];//��
    int16 Center_Repair[ROW];//�޸��������
    uint8 Center_Error[ROW];//ͼ���Ƿ�������־

    int16 LeftBorder[ROW];//��
    int16 LeftBorder_Repair[ROW];
    uint8 LeftBorderFindFlag[ROW];
    float LeftSlope[ROW];

    int16 RightBorder[ROW];//��
    int16 RightBorder_Repair[ROW];
    uint8 RightBorderFindFlag[ROW];
    float RightSlope[ROW];

    int8  RoadWide[ROW];//����ͷ��ÿ��
    int8  RoadWideError[ROW];//��������ȱ�Ƚϵõ������
    uint8 RoadNormalFlag[ROW];//�������
} ImageDealType;



//-------------������������--------------//
typedef struct
{
    int16 	 StartSearchColumn_1st;
    int16	 StartSearchColumn;
    PosiType firstleftnormal;
    PosiType firstrightnormal;
    PosiType firstleftlose;//��һ�ζ�ʧ���߽������ҵ����ߵĻ����� ���û�ж�ʧ��˵��������Ϊ0
    PosiType firstrightlose;
    uint8	 firstroadnormal;
    uint8    firstroaderror;
    uint8 	 Topline;//��ǰͼ�񶥱�
    uint8    ToplineFindFlag;
    PosiType LeftMax;
    PosiType LeftMin;
    PosiType RightMax;
    PosiType RightMin;
    PosiType CenterMin;
    PosiType CenterMax;
    uint16 	 CenterErrorCnt;
    uint8 	 element_check;
    PosiType LeftMin20up;
    PosiType LeftMax20up;
    PosiType RightMin20up;
    PosiType RightMax20up;

} BaseTypedef;


//----------------��״̬-----------------//

typedef struct
{
    uint8 AllNormal;
    uint8 LeftNormal;
    uint8 RightNormal;
    uint8 LeftLose;
    uint8 RightLose;
    uint8 AllLose;
    uint8 LeftNormalRightLose;
    uint8 LeftLoseRightNormal;
} LineStatusType; //��Ե״̬ ��ͬ���е���ͳ��

//type
typedef struct
{
    uint8 x;
    uint8 y;
    int8 dir;
} BorderType;

//��״̬ ԭʼͼ��
typedef struct
{
    uint8 firstblackY;
    uint8 firstblackYFlag;
    uint8 firstwhiteY;
    uint8 firstwhiteYFlag;
    uint8 firstblackY20up;
    uint8 firstblackY20upFlag;
    uint8 firstwhiteY20up;
    uint8 firstwhiteY20upFlag;
    uint8 Line0_30_Black;
    uint8 Line0_30_White;
    uint8 Line30_60_Black;
    uint8 Line30_60_White;
} ColumnTypedef;

//��״̬ ԭʼͼ��
typedef struct
{
    uint16 LeftToRightFirstBlack;
    uint8  LeftToRightFirstBlackFindFlag;
    uint16 RightToLeftFirstBlack;
    uint8  RightToLeftFirstBlackFindFlag;
    uint16 LeftToRightFirstWhite;
    uint8  LeftToRightFirstWhiteFindFlag;
    uint16 RightToLeftFirstWhite;
    uint8  RightToLeftFirstWhiteFindFlag;
} RowTypedef;

//statement
extern uint8 			RoadWide0[60] ;//ֱ���������
extern uint8            ov7725_threshold;
extern int16  			Xsite;
extern int16  			Ysite;
extern ImageDealType 	Img;
extern BaseTypedef 		Base;
extern LineStatusType 	LineALL, Line5_35, Line30_60;
extern uint8 			RoadWide0[60];
extern ColumnTypedef	Column10, Column117, Column64;
extern RowTypedef       Row56;
//---------------ɨ��---------------------//


//fucntion
bool 	InRange(int32 x, int32 _min, int32 _max);
uint8   TwoPoint_AddingLine(uint8 x1, uint8 y1, uint8 x2, uint8 y2, int16 *p, uint8 StopYsite);
void 	ImageProcessingStatusInit(void);
uint8   GetBorder(void);


//-------------------------------------ֱ��-------------------------------------------------//
typedef struct
{
    uint8 FindFlag;//���ڱ�־
    uint8 Beep;
    uint8 LeftLong;//��ֱ��
    uint8 RightLong;
    uint8 LeftShort;//���ֱ��
    uint8 RightShort;
    ////����
    uint8 LeftCnt;//����
    uint8 RightCnt;
    uint8 LeftCnt40up;
    uint8 RightCnt40up;
} StraightTypedef;


extern StraightTypedef 	Straight;


//fun
uint8 Stright_Judge(void);


//-------------------------------------���-------------------------------------------------//
typedef struct
{
    uint8 FindFlag;
    uint8 Beep;
    uint8 TurnLeft;//??
    uint8 TurnRight;
    ////Cnt
    uint8 LB_TR_Cnt;
    uint8 RB_TL_Cnt;
} CurveTypedef;

extern CurveTypedef   	Curve;


//fun
uint8 Curve_Judge(void);

//-------------------------------------����·��-------------------------------------------------//
typedef enum
{
    ForkOut = 0,
    ForkFound,
    ForkInTurn,
    ForkIn,
    ForkOutTurn,
    ForkRelay//����ģʽ
} ForkState;

typedef struct
{
    //ʶ��
    uint8 FindFlag;
    uint8 InState;
    uint8 StartLine;
    uint8 CenterLineBlack;
    ForkState state;
    PosiType Peak;
    uint8 PeakFindFlag;
    uint8 ForkLeftBorder[ROW];//��ɫ���ֵ����ұ�Ե
    uint8 ForkRightBorder[ROW];
    uint8 MaybeCorssFlag;
    //��¼
    uint16 ForkAmount;
    uint16 ForkNum;
    uint8 In_Direct;
    float InDis;
    float TurnInK;
    float TurnOutK;
    uint16 InSpeed;
    uint16 OutSpeed;
} ForkTypedef;

extern ForkTypedef  Fork;

extern uint32 ForkInfo[4][3];
uint8 Fork_Judge(void);
void Fork_AddingLine(void);


//-------------------------------------����-------------------------------------------------//


typedef enum
{
    BarnOutOK_Start = 0,
    BarnStart,
    BarnOutStraight,
    BarnOutTurn,
    BarnOutOK,
    BarnTurnStraight,
    BarnInTurnStraight,//��������ת�����ʻ��һС�ξ���
    BarnInTurn,
    BarnInStraight,
    BarnIn,
    BarnPass,
} BarnStateEnum;

typedef struct
{
    uint8 FindFlag;
    uint8 OutDir;//���ⷽ��
    BarnStateEnum state;
    uint16 BarnNum;//�������
    float BarnStartDistance;//�����ܵ�ֱ��һС�ξ���

    uint16 BarnFoundELCSum14Min;//�������ʶ������ֵ
    uint16 BranInNum;//��һ��ʶ���л�����ܹ� �ڶ������
    float BarnLength;//����ܹ���Ҫ��
    float BarnFindDis;//���ֳ��⵱ǰ���
    float BarnInTurnDis;//ת��������ʱ��ǰ���
    float BarnInDistanceCount;//ת����ϵ�ǰ����
    uint16 BarnInStraightSpeed;//ת��ȥ����ʻ�ٶ�

    //�ɵ�
    uint16 StartLineFoundY;//�����߿�ʼʶ����
    float  BeforeTurnRunDis;//ʶ�����һ�ξ����ٹ�
    uint16 BarnTurnCenter;//ʶ�������߸�ֵ
    uint16 BarnTurnSpeed;//ʶ����ת���ٶ�
    float  BarnInRunDis;//ת��ȥ���ܶ೤����ͣ��

} BarnTypedef;

extern uint16 BarnInInfo[2][6];
extern BarnTypedef 		Barn;


//fun
void 	BarnOut_Judge(void);
void 	Barn_Judge(void);
void 	Barn_AddingLine(void);

//-------------------------------------����-------------------------------------------------//

typedef enum
{
    CircleFound = 0,
    CircleInStraight,
    CircleCenter,
    CircleCouldInTurn,
    CircleInTurn,
    CircleIn,
    CircleOutTurn,
    CircleOutTurnOK,
    CircleOutCenter,
    CircleOutStraight,
    CircleOut,
} CircleStateEnum;


typedef struct
{
    //ʶ��
    uint8 FindFlag;
    CircleStateEnum state;
    uint8 RoundCenterY;
    PosiType LeftUpInflexion;
    PosiType RightUpInflexion;
    PosiType CircleCenter;
    //��¼
    uint16 CircleELC14;//ʶ����ֵ14
    uint16 CircleELC23;//ʶ����ֵ23
    uint16  CircleAmount;//������ 1Ȧ ��Ź�����ʱ����
    uint16  CircleNum;//���
    float  CircleEndDis;//��������
    float  DontELCJudgeDis;//ʧ�ܵ���жϾ��� �ӽ�����ʼ��
    uint8  dir;//����
    uint16 CircleR;//�뾶
    uint16 FoundOutSpeed;//ʶ���ٶ� �����ٶ�
    uint16 CenterELC;//��������ֵ
    float  CenterDis;//�е�ʶ��
    float  TurnDis;//һ�ξ����
    uint16 TurnSpeed;//0��ʧ���ٶȻ� ��0������ٶ�
    uint16 InTurnUP_Y;//�Ϲյ�Y
    float InTurnK;//ת��k
    uint16 InSpeed;//�����ٶ�
    float OutTurnK;//ת��k

} CircleTypedef;


extern uint32 CircleInfo[][9];
extern CircleTypedef 	Circle;


//fun
void 	Circle_Judge(void);
void 	Circle_AddingLine(void);



//-------------------------------------�µ�-------------------------------------------------//
typedef enum
{
    UpRamp,
    DownRamp,
    EndRamp
} RampState;

typedef struct
{
    uint8 FindFlag;
    RampState State;
    uint8 MayRampFlag;//����ͷ�б��־
    float MayRampDis;//����ͷ�б�ʱ���
    float MayRampGyroYMax;//����ͷ�б��������Ǵ���ֵ
    float MayRampFlagClearDis;//����ͷ���г������־����
    float UpRampDis;//�µ�ȷ�Ͼ���
    float RampLegth;//�µ�����
    float EndRampDis;//�µ����¾���
    uint8 RecoverFlag;//�ָ���־ ʹ�õ�Żָ�����״̬
    float RecoverDis;//�ָ��ľ���

    float DontJudgeRampDis;//���º�һ�ξ����ڲ��������µ�
    uint8 LeftSlopeSatisfy;
    uint8 RightSlopeSatisfy;
    float RampFindDis;
    float RampEndDis;
} RampTypedef;

//statement
extern RampTypedef Ramp;

//fun
void RampJudge(void);


//-------------------------------------ʮ��-------------------------------------------------//


typedef enum
{
    CrossEnd = 0,
    CrossIn,
} CrossStateEnum;


typedef struct
{
    uint8 FindFlag;
    CrossStateEnum state;
    PosiType LU_Inflexion;
    PosiType RU_Inflexion;
    uint8    LUUP_Normal;
    uint8    RUUP_Normal;
    float CrossStartDis;
    float CrossLength;
} CrossTypedef;


extern CrossTypedef Cross;


void CrsosJudge(void);
void Cross_AddingLine(void);

#endif
