#ifndef XMLPARSER_H__CE6F419D_704D_4681_AE54_974C7C238302__INCLUDED_
#define XMLPARSER_H__CE6F419D_704D_4681_AE54_974C7C238302__INCLUDED_

	#include <stdio.h>
	#include "GMaster.h"
	#include "expat/expat.h"

	typedef struct _stAttribute
	{
		char *pcName, *pcValue;
		void *pvPrevAttribute, *pvNextAttribute;
	} stAttribute;

	typedef struct _stElement
	{
		int	 iDepth;
		char *pcName, *pcValue;
		stAttribute *pstAttribute;
		void *pvChildElement, *pvParentElement, *pvPrevPeerElement, *pvNextPeerElement;
	} stElement;

	typedef struct _stXmlBase
	{
		char *pcName;
		stElement *pcElement;
	} stXmlBase;

	typedef struct _stXMLParserMainData
	{
		stElement *pstCurElement, *pstMasElement;
		stAttribute *pstCurAttribute;
		char *pcXmlFileName;
		int iDepth;
	} stXMLParserMainData;

// expat use function pointer
	void XMLCALL fpStartElement(void *userData, const char *name, const char **atts);
	void XMLCALL fpEndElement(void *userData, const char *name);
	void XMLCALL fpDefaultElementHandler (void *userData, const char *s, int len);

// xml file reader
	unsigned char OpenXmlFile (unsigned char *p_pcFileName, char **p_ppcXmlBuf);

// data allocation function
	void AddPeerElement (stXMLParserMainData *p_pstMainXMLData, stElement *p_pstNewElement);
	void AddChildElement (stXMLParserMainData *p_pstMainXMLData, stElement *p_pstNewElement);
	void AddAttribute (stXMLParserMainData *p_pstMainXMLData, stAttribute *p_pstNewAtrribute);
	void ClearBuffer (stXMLParserMainData *p_pstMainXMLData);

// external function
// Main function
	void InitXmlParser (stXMLParserMainData *p_pstMainXMLData, char *p_pcXMLFileName);
	unsigned char ParserXml (stXMLParserMainData *p_pstMainXMLData);
	void TerminateXmlParser (stXMLParserMainData *p_pstMainXMLData);

// element explore function
	stElement *GetBaseElement (stXMLParserMainData *p_pstMainXMLData);
	stElement *GetLastChildElement(stElement  *p_pstCurElement);
	stElement *GetLastPeerElement (stElement *p_pstCurElement);
	stElement *GetLastElement (stElement *p_pstCurElement);

	stElement *GetNextChild (stElement *p_pstElement);
	stElement *GetNextPeer (stElement *p_pstElement);
	stElement *FindChildByName (stElement *p_pstElement, char *p_pcNodeName, char *p_pcAttrName, char *p_pcAttrVal);
	stElement *FindChildElementByName (stXMLParserMainData *p_pstMainXMLData, stElement *p_pstElement, 
								 char *p_pcFullName, char *p_pcAttrName, char *p_pcAttrVal);

	stAttribute *GetNextAttribute (stAttribute *p_pstAttribute);
	unsigned char FindAttributeValue (stAttribute *p_pstAttribute, char *p_pcAttrName, char **p_ppcAttrValue);
	int GetTotalChild(stElement *p_pstElement, char *p_pcName);
	int GetTotalAttribute(stElement *p_pstElement);
	int GetTotalSamePeer(stElement *p_pstElement);
#endif
