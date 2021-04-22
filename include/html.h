
#define MAX_VARIABLE_CNT_IN_PAGE 10

typedef struct 
{
  String htmlTxt;
  int variableCount;
  char* variableStrings[MAX_VARIABLE_CNT_IN_PAGE];
} HtmlStruct;
