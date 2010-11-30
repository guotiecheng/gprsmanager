#include "CXmlParser.h"

//================================
//   xpat use function pointer  //
//================================
void XMLCALL fpStartElement(void *userData, const char *name, const char **atts)
{
	int i;
	stXMLParserMainData *pstMainXMLData = (stXMLParserMainData *)userData;
	stElement *pNewElement, *pTemp;
	stAttribute *pNewAttribute;

	MEM_ALLOC (pNewElement, sizeof (stElement), (stElement *));

	STR_COPY (pNewElement->pcName, name);

	pNewElement->iDepth = pstMainXMLData->iDepth;

	pstMainXMLData->pstCurAttribute = 0;

	for (i = 0; atts[i]; i += 2)
	{
		MEM_ALLOC (pNewAttribute, sizeof (stAttribute), (stAttribute *));
		STR_COPY (pNewAttribute->pcName, atts [i]);
		STR_COPY (pNewAttribute->pcValue, atts [i + 1]);

		if (0 == i)
		  pNewElement->pstAttribute = pNewAttribute;

		AddAttribute (pstMainXMLData, pNewAttribute);
		//   printf(" %s:'%s'\r\n", atts[i], atts[i + 1]);
	}

	if (0 != pstMainXMLData->pstCurElement)
	{
		if (0 == pstMainXMLData->pstCurElement->pvChildElement)
			AddChildElement (pstMainXMLData, pNewElement);
		else
		{
			pTemp = GetLastPeerElement (pstMainXMLData->pstCurElement->pvChildElement);
			if (pTemp)
			{
				pstMainXMLData->pstCurElement = pTemp;
			}
			else
			{
				pstMainXMLData->pstCurElement = pstMainXMLData->pstCurElement->pvChildElement;
			}
			AddPeerElement (pstMainXMLData, pNewElement);
		}
	}
	else
		AddChildElement (pstMainXMLData, pNewElement);

	pstMainXMLData->iDepth += 1;
}

void XMLCALL fpEndElement(void *userData, const char *name)
{
	stXMLParserMainData *pstMainXMLData = (stXMLParserMainData *)userData;

	pstMainXMLData->iDepth -= 1;
	pstMainXMLData->pstCurElement = pstMainXMLData->pstCurElement->pvParentElement;
}

void XMLCALL fpDefaultElementHandler (void *userData, const char *s, int len)
{
	stXMLParserMainData *pstMainXMLData = (stXMLParserMainData *)userData;

	if (1 == pstMainXMLData->iDepth)
		return;

	if (0 != pstMainXMLData->pstCurElement)
	{
		// Never set the value before
		if (0 == pstMainXMLData->pstCurElement->pcValue)
		{
			MEM_ALLOC (pstMainXMLData->pstCurElement->pcValue, len + 1, (char *));
			strncpy (pstMainXMLData->pstCurElement->pcValue, s, len);
		}
	}
}
//===============================
//   xpat use function pointer //
//===============================

unsigned char OpenXmlFile (unsigned char *p_pcFileName, char **p_ppcXmlBuf)
{
	unsigned long	ulCurSize;

	FILE *pFile;

	pFile = fopen((char *) p_pcFileName, "rb");

	if (0 == pFile)
	{
		return 0x01;
	}

	fseek (pFile, 0, SEEK_END);
	ulCurSize = ftell (pFile);

	fseek (pFile, 0, SEEK_SET);

	if (0 != *p_ppcXmlBuf)
	{
		MEM_CLEAN (*p_ppcXmlBuf);
	}

	MEM_ALLOC (*p_ppcXmlBuf, ulCurSize + 1, ( char* ) );

	fread (*p_ppcXmlBuf, 1, ulCurSize, pFile);

	fclose(pFile);

	return 0x00;
}

void ClearBuffer (stXMLParserMainData *p_pstMainXMLData)
{
	stElement  *pElement, *pParent;
	stAttribute *pAttribute, *pTemp;

	if (0 != p_pstMainXMLData->pstMasElement)
	{
		// Clean up all Child Element
		while (p_pstMainXMLData->pstMasElement->pvChildElement)
		{
			// Get Last Element
			pElement = GetLastElement(p_pstMainXMLData->pstMasElement);

			// Get Last Element Attribute
			pAttribute = pElement->pstAttribute;
			if (pAttribute)
			{
				// Find Last Attribute
				while (pAttribute)
				{
					if (0 == pAttribute->pvNextAttribute)
						break;

					pAttribute = pAttribute->pvNextAttribute;
				}

				// Clean up from last Attribute
				while (pAttribute->pvPrevAttribute)
				{
					pAttribute = pAttribute->pvPrevAttribute;

					pTemp = pAttribute->pvNextAttribute;

					// Clean up attribute
					MEM_CLEAN (pTemp->pcName);
					MEM_CLEAN (pTemp->pcValue);
					MEM_CLEAN (pTemp->pvNextAttribute);
					MEM_CLEAN (pAttribute->pvNextAttribute);
				}

				// Clean up first and final Attribute
				MEM_CLEAN (pAttribute->pcName);
				MEM_CLEAN (pAttribute->pcValue);
				MEM_CLEAN (pAttribute->pvPrevAttribute);
			}

			// Clean up Element
			MEM_CLEAN (pElement->pcName);
			MEM_CLEAN (pElement->pcValue);
			MEM_CLEAN (pElement->pstAttribute);

			// Get peer or parent to clean up current Element
			pParent = pElement->pvPrevPeerElement; 
			if (pParent)
			{
				MEM_CLEAN (pParent->pvNextPeerElement);
			}
			else
			{
				pParent = pElement->pvParentElement;
				if (pParent)
				{
					MEM_CLEAN (pParent->pvChildElement);
				}
				else
				{
					MEM_CLEAN (p_pstMainXMLData->pstMasElement->pvChildElement);
				}
			}
		}

		// Clean up Master Element
		pAttribute = p_pstMainXMLData->pstMasElement->pstAttribute;
		if (pAttribute)
		{
			// Find the Last Attribute
			while (pAttribute)
			{
				if (0 == pAttribute->pvNextAttribute)
					break;

				pAttribute = pAttribute->pvNextAttribute;
			}

			// Clean up from Last Attribute
			while (pAttribute->pvPrevAttribute)
			{
				pAttribute = pAttribute->pvPrevAttribute;

				pTemp = pAttribute->pvNextAttribute;

				MEM_CLEAN (pTemp->pcName);
				MEM_CLEAN (pTemp->pcValue);
				MEM_CLEAN (pTemp->pvNextAttribute);
				MEM_CLEAN (pTemp->pvPrevAttribute);
				MEM_CLEAN (pAttribute->pvNextAttribute);
			}

			MEM_CLEAN (pAttribute->pcName);
			MEM_CLEAN (pAttribute->pcValue);
			MEM_CLEAN (pAttribute->pvPrevAttribute);
		}

 		MEM_CLEAN (p_pstMainXMLData->pstMasElement->pcName);
		MEM_CLEAN (p_pstMainXMLData->pstMasElement->pcValue);
		MEM_CLEAN (p_pstMainXMLData->pstMasElement->pstAttribute);
		MEM_CLEAN (p_pstMainXMLData->pstMasElement->pvChildElement);
		MEM_CLEAN (p_pstMainXMLData->pstMasElement->pvParentElement);
		MEM_CLEAN (p_pstMainXMLData->pstMasElement->pvNextPeerElement);
		MEM_CLEAN (p_pstMainXMLData->pstMasElement->pvPrevPeerElement);

		MEM_CLEAN (p_pstMainXMLData->pstMasElement);
	}

	MEM_CLEAN (p_pstMainXMLData->pcXmlFileName);
}

void AddPeerElement (stXMLParserMainData *p_pstMainXMLData, stElement *p_pstNewElement)
{
	p_pstNewElement->pvParentElement = p_pstMainXMLData->pstCurElement->pvParentElement;
	p_pstNewElement->pvPrevPeerElement = p_pstMainXMLData->pstCurElement;

	p_pstMainXMLData->pstCurElement->pvNextPeerElement = p_pstNewElement;

	p_pstMainXMLData->pstCurElement = p_pstNewElement;
}

void AddChildElement (stXMLParserMainData *p_pstMainXMLData, stElement *p_pstNewElement)
{
	if (0 == p_pstMainXMLData->pstCurElement)
	{
		p_pstMainXMLData->pstCurElement = p_pstNewElement;

		p_pstMainXMLData->pstMasElement = p_pstMainXMLData->pstCurElement;
	}
	else
	{
		p_pstNewElement->pvParentElement = p_pstMainXMLData->pstCurElement;
		p_pstMainXMLData->pstCurElement->pvChildElement = p_pstNewElement;

		p_pstMainXMLData->pstCurElement = p_pstNewElement;
	}
}

void AddAttribute (stXMLParserMainData *p_pstMainXMLData, stAttribute *p_pstNewAtrribute)
{
	if (0 == p_pstMainXMLData->pstCurAttribute)
	{
		p_pstMainXMLData->pstCurAttribute = p_pstNewAtrribute;
	}
	else
	{
		p_pstNewAtrribute->pvPrevAttribute = p_pstMainXMLData->pstCurAttribute;
		p_pstMainXMLData->pstCurAttribute->pvNextAttribute = p_pstNewAtrribute;
		p_pstMainXMLData->pstCurAttribute = p_pstMainXMLData->pstCurAttribute->pvNextAttribute;
	}
}

stElement *GetBaseElement (stXMLParserMainData *p_pstMainXMLData)
{
	return (0 != p_pstMainXMLData) ? p_pstMainXMLData->pstMasElement : 0;
}

stElement *GetLastElement (stElement *p_pstCurElement)
{
	stElement *pElement = p_pstCurElement;

	if (0 != pElement)
	{
		while (pElement->pvChildElement)
		{
			pElement = GetLastChildElement (pElement);

			pElement = GetLastPeerElement (pElement);
		}
	}

	return pElement;
}

stElement *GetLastChildElement(stElement  *p_pstCurElement)
{
	if (0 != p_pstCurElement)
	{
		while (p_pstCurElement->pvChildElement)
		{
			p_pstCurElement = p_pstCurElement->pvChildElement;
		}
	}

	return p_pstCurElement;
}

stElement *GetLastPeerElement (stElement *p_pstCurElement)
{
	if (0 != p_pstCurElement)
	{
		while (p_pstCurElement->pvNextPeerElement)
		{
			p_pstCurElement = p_pstCurElement->pvNextPeerElement;
		}
	}

	return p_pstCurElement;
}

stElement *GetNextChild (stElement *p_pstElement)
{
	return (0 != p_pstElement) ? p_pstElement->pvChildElement : 0;
}

stElement *GetNextPeer (stElement *p_pstElement)
{
	return (0 != p_pstElement) ? p_pstElement->pvNextPeerElement : 0;
}

stElement *FindChildByName (stElement *p_pstElement, char *p_pcNodeName, char *p_pcAttrName, char *p_pcAttrVal)
{
	stElement *pElement = 0;
	stAttribute *pAttribute = 0;

	if (0 == p_pstElement)
	{
		goto CleanUp;
	}

	p_pstElement = p_pstElement->pvChildElement;

	while (p_pstElement)
	{
		if (0 == strcmp (p_pstElement->pcName, p_pcNodeName))
		{
			if (0 == p_pcAttrName)
			{
				pElement = p_pstElement;
				break;
			}

			pAttribute = p_pstElement->pstAttribute;
			while (pAttribute)
			{
				if (0 != pAttribute->pcName && 0 != pAttribute->pcValue)
				{
					if (0 == strcmp(pAttribute->pcName, p_pcAttrName) &&
						0 == strcmp(pAttribute->pcValue, p_pcAttrVal))
					{
						pElement = p_pstElement;
						goto CleanUp;
					}
				}
				pAttribute = pAttribute->pvNextAttribute;
			}
		}
		p_pstElement = GetNextPeer (p_pstElement);
	}

CleanUp:
	return pElement;
}

stElement *FindChildElementByName (stXMLParserMainData *p_pstMainXMLData, stElement *p_pstElement, 
								 char *p_pcFullName, char *p_pcAttrName, char *p_pcAttrVal)
{
	int iSeperatorPsn;
	char *pcSeperator, acName[256];
	stElement *pChildElement = 0;

	if ('.' != *p_pcFullName && 0 == p_pstElement)
	{
		return 0;
	}
	else
	{
		if ('.' == *p_pcFullName)
		{
			p_pcFullName += 1;
		}
		else
		{
			p_pstElement = p_pstMainXMLData->pstMasElement;
		}
	}

	pChildElement = p_pstElement;

	if (0 == pChildElement)
	{
		return 0;
	}

	do
	{
		pcSeperator = strchr(p_pcFullName, '.');
		iSeperatorPsn = pcSeperator - p_pcFullName;


		if (0 > iSeperatorPsn)
		{
			break;
		}

		strncpy (acName, p_pcFullName, iSeperatorPsn);
		acName[iSeperatorPsn] = 0;

		if (0 != strcmp(pChildElement->pcName, acName))
		{
			return 0;
		}
		else
		{
			pChildElement = pChildElement->pvChildElement;
		}

		if (0 == pChildElement)
		{
			return 0;
		}

		p_pcFullName += (iSeperatorPsn + 1);

	} while (0 <= iSeperatorPsn);

	if (0 != strlen(p_pcFullName))
	{
		pChildElement = FindChildByName (pChildElement->pvParentElement, p_pcFullName, p_pcAttrName, p_pcAttrVal);
		if (0 == pChildElement)
		{
			return 0;
		}
	}

	return pChildElement;
}

stAttribute *GetNextAttribute (stAttribute *p_pstAttribute)
{
	return (0 != p_pstAttribute) ? p_pstAttribute->pvNextAttribute : 0;
}

unsigned char FindAttributeValue (stAttribute *p_pstAttribute, char *p_pcAttrName, char **p_ppcAttrValue)
{
	while (p_pstAttribute)
	{
		if (0 != p_pstAttribute->pcName && 0 != p_pstAttribute->pcValue)
		{
			if (0 == strcmp(p_pstAttribute->pcName, p_pcAttrName))
			{
				*p_ppcAttrValue = p_pstAttribute->pcValue;

				return 0x00;
			}
		}
		p_pstAttribute = p_pstAttribute->pvNextAttribute;
	}

	return 0x01;
}


void InitXmlParser(stXMLParserMainData *p_pstMainXMLData, char *p_pcFileName)
{
	p_pstMainXMLData->pstMasElement = 0;
	p_pstMainXMLData->pstCurElement = 0;
	p_pstMainXMLData->pstCurAttribute = 0;

	STR_COPY (p_pstMainXMLData->pcXmlFileName, p_pcFileName);

	p_pstMainXMLData->iDepth = 0;
}

int GetTotalChild(stElement *p_pstElement, char *p_pcName)
{
	int iPeer = 0;

	stElement *pPeer;

	pPeer = GetNextChild(p_pstElement);

	while (pPeer)
	{
		if (0 != p_pcName)
		{
			if (0 == strcmp (pPeer->pcName, p_pcName))
			{
				iPeer ++;
			}
		}

		pPeer = GetNextPeer(pPeer);
	}

	return iPeer;
}

int GetTotalSamePeer(stElement *p_pstElement)
{
	int iPeer = 0;

	char *pcPeerName = 0; 

	stElement *pPeer = 0;

	if(p_pstElement)
	{
		pcPeerName = p_pstElement->pcName;

		iPeer +=1; 
		pPeer = GetNextPeer(p_pstElement);

		while(pPeer)
		{
			if(0==strcmp(pcPeerName,pPeer->pcName))
			{
				iPeer ++;
			}

			pPeer = GetNextPeer(pPeer);
		}
	}
	else
	{
		iPeer = 0;
	}

	return iPeer;	
}

int GetTotalAttribute(stElement *p_pstElement)
{
	int iAttr = 0;

	stAttribute *pAttr;

	pAttr = GetNextAttribute(p_pstElement->pstAttribute);

	while (pAttr)
	{
		iAttr ++;
		
		pAttr = GetNextAttribute(pAttr);
	}

	return iAttr;
}

unsigned char ParserXml (stXMLParserMainData *p_pstMainXMLData)
{
	XML_Parser		XMLParser;
	unsigned char	ucRet;
	char			*pcXMLBuf = 0;

	if (0 == p_pstMainXMLData->pcXmlFileName)
	{
		ucRet = 0x01;
		goto CleanUp;
	}

	ucRet = OpenXmlFile((unsigned char *) p_pstMainXMLData->pcXmlFileName, &pcXMLBuf);

	if (0x00 != ucRet)
	{
		goto CleanUp;
	}

	XMLParser = XML_ParserCreate(0);

	if (0 == XMLParser)
	{
		ucRet = 0x01;
		goto CleanUp;
	}

	// Set Depth variable
	XML_SetUserData(XMLParser, p_pstMainXMLData);
	// Set Parser Start and End function pointer handler
	XML_SetElementHandler(XMLParser, fpStartElement, fpEndElement);
	// Set Parser Default function pointer handler
	XML_SetDefaultHandler (XMLParser, fpDefaultElementHandler);

	if (!XML_Parse (XMLParser, pcXMLBuf, strlen(pcXMLBuf), 1))
	{
		ucRet = 0x02;
		goto CleanUp;
	}
	else
	{
		XML_ParserFree (XMLParser);
	}

CleanUp:
	MEM_CLEAN (pcXMLBuf);

	return ucRet;
}

void TerminateXmlParser (stXMLParserMainData *p_pstMainXMLData)
{
	ClearBuffer (p_pstMainXMLData);
}
