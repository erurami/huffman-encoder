
// (M)ainWindow (M)essage
// 500 - 599
#define MM_INPUTSELECTED  0x500
#define MM_OUTPUTSELECTED 0x501
#define MM_GENERATEOUTPUT 0x502
#define MM_SETINPUT       0x503
#define MM_STARTENCODE    0x504
#define MM_OPENFILE       0x505
#define MM_SETENCODETYPE  0x506


// (C)ompression(P)rogress (M)essage
// 600-699
#define CPM_REGISTERPARENT   0x0600
#define CPM_STARTCOMPRESS    0x0601
#define CPM_UPDATEPROGRESS   0x0602
#define CPM_ENDCOMPRESSION   0x0603


// (E)xtraction(P)rogress (M)essage
// 700-799
#define EPM_REGISTERPARENT 0x0700
#define EPM_STARTEXTRACT   0x0701
#define EPM_UPDATEPROGRESS 0x0702
#define EPM_ENDEXTRACTION  0x0703

// (Comp)ression endcode
#define _COMP_ENDCODE_SUCCESS               1
#define _COMP_ENDCODE_ERROR                 2
#define _COMP_ENDCODE_FAILED_OPENFILE       3
#define _COMP_ENDCODE_USER_CANCEL           4

// (Ext)ra(c)tion endcode
#define _EXTC_ENDCODE_SUCCESS               1
#define _EXTC_ENDCODE_ERROR                 2
#define _EXTC_ENDCODE_FAILED_OPENFILE       3
#define _EXTC_ENDCODE_USER_CANCEL           4
