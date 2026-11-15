#include <libxml/parser.h>
#include <libxml/tree.h>
#include <stdio.h>
#include <stdlib.h>
#include <uthash.h>

typedef struct {
  char name[32];
  xmlNodePtr nodePtr;
  UT_hash_handle hh;
} XmlFunctionStruct;

typedef struct {
  char name[32];
  char *val;
  UT_hash_handle hh;
} Script_VarStruct;

XmlFunctionStruct *userFunctions = NULL;
Script_VarStruct *scriptVariables = NULL;
void addIntToCurrentScope(const char *name, int val);
void setIntValue(const char *name, int val);

char *extractValueFromVar(xmlChar *val) {
  char *varname = calloc(0, strlen(val));
  sscanf(val, "${%[^}]}", varname);
  Script_VarStruct *v;
  HASH_FIND_STR(scriptVariables, varname, v);
  free(varname);
  return v->val;
}

void parseScope(xmlNodePtr nodePtr) {
  while (nodePtr != NULL) {
    // puts((const char *)nodePtr->name);
    if (!xmlStrcmp(nodePtr->name, (const xmlChar *)"if")) {
      xmlChar *lval = xmlGetProp(nodePtr, (xmlChar *)"lval");
      xmlChar *rval = xmlGetProp(nodePtr, (xmlChar *)"rval");
      if (lval == NULL || rval == NULL) {
        return;
      }
      if (lval[0] == '$') {
        lval = extractValueFromVar(lval);
      }
      if (rval[0] == '$') {
        rval = extractValueFromVar(rval);
      }
      if (!xmlStrcmp(lval, rval)) {
        parseScope(nodePtr->children);
      }
    }
    if (!xmlStrcmp(nodePtr->name, (const xmlChar *)"for")) {
      xmlChar *var = xmlGetProp(nodePtr, (xmlChar *)"var");
      xmlChar *start = xmlGetProp(nodePtr, (xmlChar *)"start");
      xmlChar *end = xmlGetProp(nodePtr, (xmlChar *)"end");
      xmlChar *step = xmlGetProp(nodePtr, (xmlChar *)"step");
      if (var == NULL || start == NULL || end == NULL || step == NULL) {
        return;
      }
      int _var = atoi(var);
      int _start = atoi(start);
      int _end = atoi(end);
      int _step = atoi(step);
      addIntToCurrentScope("i", _start);
      /*if (lval[0] == '$') {
        lval = extractValueFromVar(lval);
      }
      if (rval[0] == '$') {
        rval = extractValueFromVar(rval);
      }*/
      for (int i = _var; i < _end; i = i + _step) {
        parseScope(nodePtr->children);
        setIntValue("i", i + _step);
      }
    }
    if (!xmlStrcmp(nodePtr->name, (const xmlChar *)"print")) {
      xmlChar *msg = xmlGetProp(nodePtr, (xmlChar *)"msg");
      if (msg != NULL) {
        for (unsigned int i = 0; msg[i] != NULL; i = i + 1) {
          if (msg[i] == '$') {
            char *ss;
            int cnt = 0;
            for (unsigned int j = 0; msg[i + j] != '}'; j = j + 1) {
              cnt = cnt + 1;
            }
            ss = calloc(0, sizeof(char) * cnt);
            strncpy(ss, msg + i, cnt);
            char *val = extractValueFromVar(ss);
            printf("%s", val);
            i = i + cnt + 1;
          }
          fprintf(stdout, "%c", msg[i]);
        }
        fprintf(stdout, "\n");
      }
    }
    if (!xmlStrcmp(nodePtr->name, (const xmlChar *)"set")) {
      xmlChar *varc = xmlGetProp(nodePtr, (xmlChar *)"var");
      xmlChar *vc = xmlGetProp(nodePtr, (xmlChar *)"v");
      xmlChar *slotc = xmlGetProp(nodePtr, (xmlChar *)"slot");
      if (slotc == NULL) { /* We are not dealing with an array */
        char *val = extractValueFromVar(varc);
        val = strcpy(val, vc);
      } else {
        char *val = extractValueFromVar(varc);
        val[atoi(slotc)] = atoi(vc);
      }
    }
    XmlFunctionStruct *s;
    HASH_FIND_STR(userFunctions, nodePtr->name, s);
    if (s) {
      parseScope(s->nodePtr->children);
    }

    nodePtr = nodePtr->next;
  }
}

static void example3Func() {
  xmlDocPtr doc; /* the resulting document tree */
  doc = xmlReadFile("TESTSCRIPT.XML", NULL, 0);
  if (doc == NULL) {
    fprintf(stderr, "Failed to parse document\n");
    return;
  }
  xmlNodePtr cur = xmlDocGetRootElement(doc);
  if (cur == NULL) {
    fprintf(stderr, "empty document");
    xmlFreeDoc(doc);
    return;
  }
  if (xmlStrcmp(cur->name, (const xmlChar *)"script")) {
    fprintf(stderr, "XML is not in the right format, root_node != script");
    xmlFreeDoc(doc);
    return;
  }

  xmlNodePtr collectFunctions = xmlDocGetRootElement(doc);
  collectFunctions = collectFunctions->xmlChildrenNode;
  userFunctions = NULL;
  int fCount = 0;
  while (collectFunctions != NULL) {
    if (!xmlStrcmp(collectFunctions->name, (const xmlChar *)"function")) {
      fCount = fCount + 1;
      XmlFunctionStruct *f = NULL;
      f = malloc(sizeof *f);
      f->nodePtr = collectFunctions;
      strcpy(f->name, (char *)xmlGetProp(collectFunctions, "name"));
      HASH_ADD_STR(userFunctions, name, f);
    }
    if (!xmlStrcmp(collectFunctions->name, (const xmlChar *)"var")) {
      Script_VarStruct *v = NULL;
      xmlChar *widthc = xmlGetProp(collectFunctions, (xmlChar *)"width");
      v = malloc(sizeof *v);
      if (widthc == NULL) {
        v->val =
            malloc(sizeof(int) * strlen(xmlGetProp(collectFunctions, "v")));
      } else {
        v->val = calloc(0, sizeof(char) * atoi(widthc));
      }
      strcpy(v->name, xmlGetProp(collectFunctions, "name"));
      strcpy(v->val, xmlGetProp(collectFunctions, "v"));
      HASH_ADD_STR(scriptVariables, name, v);
    }
    collectFunctions = collectFunctions->next;
  }

  xmlNodePtr mainFuncTag = xmlDocGetRootElement(doc);
  mainFuncTag = mainFuncTag->xmlChildrenNode;
  while (mainFuncTag != NULL) {
    if (!xmlStrcmp(mainFuncTag->name, (const xmlChar *)"main")) {
      parseScope(mainFuncTag->children);
    }
    mainFuncTag = mainFuncTag->next;
  }

  xmlFreeDoc(doc);
}

void addIntToCurrentScope(const char *name, int val) {
  Script_VarStruct *v = NULL;
  v = malloc(sizeof *v);
  v->val = malloc(sizeof(int));
  strcpy(v->name, name); // strcpy(v->val, itoa(val));
  sprintf(v->val, "%d", val);
  HASH_ADD_STR(scriptVariables, name, v);
}

void setIntValue(const char *name, int val) {
  Script_VarStruct *v = NULL;
  HASH_FIND_STR(scriptVariables, name, v);
  sprintf(v->val, "%d", val);
}

int main(void) {
  LIBXML_TEST_VERSION

  example3Func();

  Script_VarStruct *s;

  for (s = scriptVariables; s != NULL; s = (Script_VarStruct *)(s->hh.next)) {
    free(s->val);
  }
  {
    Script_VarStruct *current_user;
    Script_VarStruct *tmp;

    HASH_ITER(hh, scriptVariables, current_user, tmp) {
      HASH_DEL(scriptVariables,
               current_user); /* delete it (users advances to next) */
      free(current_user);     /* free it */
    }
  }
  {
    XmlFunctionStruct *current_user;
    XmlFunctionStruct *tmp;
    HASH_ITER(hh, userFunctions, current_user, tmp) {
      HASH_DEL(userFunctions,
               current_user); /* delete it (users advances to next) */
      free(current_user);     /* free it */
    }
  }
  return (0);
}
