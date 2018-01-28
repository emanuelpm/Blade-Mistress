
// ******************************************************************
// each user definition has karma counters
...

enum
{
	SAMARITAN_REL_OLDER,
	SAMARITAN_REL_PEER,
	SAMARITAN_REL_YOUNGER,
	SAMARITAN_REL_GUILD,
	SAMARITAN_REL_MAX
};

enum
{
	SAMARITAN_TYPE_THANKS,
	SAMARITAN_TYPE_PLEASE,
	SAMARITAN_TYPE_WELCOME,
	SAMARITAN_TYPE_GIFT,
	SAMARITAN_TYPE_CASH,
	SAMARITAN_TYPE_MAX
};


	int karmaGiven   [SAMARITAN_REL_MAX][SAMARITAN_TYPE_MAX];
	int karmaReceived[SAMARITAN_REL_MAX][SAMARITAN_TYPE_MAX];

...

// ******************************************************************
// when creating a new character, karma counters are zeroed out
...
	for (int i = 0; i < NUM_OF_CHARS_PER_USER; ++i)
	{
		charInfoArray[i].petDragonInfo[0].lastAttackTime = 0;
		charInfoArray[i].petDragonInfo[1].lastAttackTime = 0;

		for (int j = 0; j < SAMARITAN_REL_MAX; ++j)
		{
			for (int k = 0; k < SAMARITAN_TYPE_MAX; ++k)
			{
				charInfoArray[i].karmaGiven[j][k] = 0;
				charInfoArray[i].karmaReceived[j][k] = 0;
			}
		}

	}
...

// ******************************************************************
// karma counters are loaded
...
		if (fileVersionNumber >= 1.79f)
		{
			for (int j = 0; j < SAMARITAN_REL_MAX; ++j)
			{
				for (int k = 0; k < SAMARITAN_TYPE_MAX; ++k)
				{
					fscanf(fp,"%d %d", &charInfoArray[i].karmaGiven[j][k], &charInfoArray[i].karmaReceived[j][k]);
				}
			}
		}

...

// ******************************************************************
// and saved
...
			for (int j = 0; j < SAMARITAN_REL_MAX; ++j)
			{
				for (int k = 0; k < SAMARITAN_TYPE_MAX; ++k)
				{
					fprintf(fp," %d %d", charInfoArray[i].karmaGiven[j][k], charInfoArray[i].karmaReceived[j][k]);
				}
			}
			fprintf(fp,"\n");
...

//******************************************************************
// when a secure trade is consumated, karma is apportioned
...
			// KARMA
			int myRelationship, herRelationship;

			CompareWith(partnerAvatar, myRelationship, herRelationship);

			if (sheGave && !iGave)
			{
				partnerAvatar->charInfoArray[partnerAvatar->curCharacterIndex].
					  karmaGiven[herRelationship][SAMARITAN_TYPE_GIFT] += 1;
				charInfoArray[curCharacterIndex].
					  karmaReceived[myRelationship][SAMARITAN_TYPE_GIFT] += 1;

				partnerAvatar->LogKarmaExchange(
					       this, herRelationship, myRelationship, SAMARITAN_TYPE_GIFT);

				if (IsAGuildMate(partnerAvatar))
				{
					partnerAvatar->charInfoArray[partnerAvatar->curCharacterIndex].
						  karmaGiven[SAMARITAN_REL_GUILD][SAMARITAN_TYPE_GIFT] += 1;
					charInfoArray[curCharacterIndex].
						  karmaReceived[SAMARITAN_REL_GUILD][SAMARITAN_TYPE_GIFT] += 1;

					partnerAvatar->LogKarmaExchange(
					     this, SAMARITAN_REL_GUILD, SAMARITAN_REL_GUILD, SAMARITAN_TYPE_GIFT);
				}
			}
			if (!sheGave && iGave)
			{
				partnerAvatar->charInfoArray[partnerAvatar->curCharacterIndex].
					  karmaReceived[herRelationship][SAMARITAN_TYPE_GIFT] += 1;
				charInfoArray[curCharacterIndex].
					  karmaGiven[myRelationship][SAMARITAN_TYPE_GIFT] += 1;

				LogKarmaExchange(partnerAvatar,
					       myRelationship, herRelationship, SAMARITAN_TYPE_GIFT);

				if (IsAGuildMate(partnerAvatar))
				{
					partnerAvatar->charInfoArray[partnerAvatar->curCharacterIndex].
						  karmaReceived[SAMARITAN_REL_GUILD][SAMARITAN_TYPE_GIFT] += 1;
					charInfoArray[curCharacterIndex].
						  karmaGiven[SAMARITAN_REL_GUILD][SAMARITAN_TYPE_GIFT] += 1;

					LogKarmaExchange(partnerAvatar,
					     SAMARITAN_REL_GUILD, SAMARITAN_REL_GUILD, SAMARITAN_TYPE_GIFT);
				}
			}
...

//******************************************************************
// when gold is given...
								// KARMA
								int myRelationship, herRelationship;

								curAvatar->CompareWith(destAvatar, myRelationship, herRelationship);

								destAvatar->charInfoArray[destAvatar->curCharacterIndex].
									  karmaReceived[herRelationship][SAMARITAN_TYPE_CASH] += 1;
								curAvatar->charInfoArray[curAvatar->curCharacterIndex].
									  karmaGiven[myRelationship][SAMARITAN_TYPE_CASH] += 1;

								curAvatar->LogKarmaExchange(destAvatar,
							       myRelationship, herRelationship, SAMARITAN_TYPE_CASH);

								if (curAvatar->IsAGuildMate(destAvatar))
								{
									destAvatar->charInfoArray[destAvatar->curCharacterIndex].
										  karmaReceived[SAMARITAN_REL_GUILD][SAMARITAN_TYPE_CASH] += 1;
									curAvatar->charInfoArray[curAvatar->curCharacterIndex].
										  karmaGiven[SAMARITAN_REL_GUILD][SAMARITAN_TYPE_CASH] += 1;

									curAvatar->LogKarmaExchange(destAvatar,
										 SAMARITAN_REL_GUILD, SAMARITAN_REL_GUILD, SAMARITAN_TYPE_CASH);
								}
								// END KARMA




//******************************************************************
void BBOSAvatar::LogKarmaExchange(BBOSAvatar *receiver, 
											 int myRel, int receiverRel, int exchangeType,
											 char *originalText)
{
	char tempText[1024];
	LongTime lt;

	sprintf(tempText,"%d/%02d, %d:%02d,    ", (int)lt.value.wMonth, (int)lt.value.wDay, 
			  (int)lt.value.wHour, (int)lt.value.wMinute);
	LogOutput("karma.txt", tempText);

	sprintf(tempText,"%s, ", charInfoArray[curCharacterIndex].name);
	LogOutput("karma.txt", tempText);

	switch(myRel)
	{
	case SAMARITAN_REL_OLDER:
		LogOutput("karma.txt", "ELDER, ");
		break;

	case SAMARITAN_REL_PEER:
		LogOutput("karma.txt", "PEER, ");
		break;

	case SAMARITAN_REL_YOUNGER:
		LogOutput("karma.txt", "YOUNGER, ");
		break;

	case SAMARITAN_REL_GUILD:
		LogOutput("karma.txt", "GUILDMATE, ");
		break;

	default:
		LogOutput("karma.txt", "NO_RELATIONSHIP, ");
		break;
	}

	switch(exchangeType)
	{
	case SAMARITAN_TYPE_THANKS:
		LogOutput("karma.txt", "SAYS_THANKS_TO, ");
		break;

	case SAMARITAN_TYPE_PLEASE:
		LogOutput("karma.txt", "SAYS_PLEASE_TO, ");
		break;

	case SAMARITAN_TYPE_WELCOME:
		LogOutput("karma.txt", "SAYS_WELCOME_TO, ");
		break;

	case SAMARITAN_TYPE_GIFT:
		LogOutput("karma.txt", "GIVES_SECURE_GIFT_TO, ");
		break;

	case SAMARITAN_TYPE_CASH:
		LogOutput("karma.txt", "GIVES_CASH_TO, ");
		break;

	default:
		LogOutput("karma.txt", "NO_ACTION, ");
		break;
	}


	sprintf(tempText,"%s, ", receiver->charInfoArray[receiver->curCharacterIndex].name);
	LogOutput("karma.txt", tempText);

	switch(receiverRel)
	{
	case SAMARITAN_REL_OLDER:
		LogOutput("karma.txt", "ELDER, ");
		break;

	case SAMARITAN_REL_PEER:
		LogOutput("karma.txt", "PEER, ");
		break;

	case SAMARITAN_REL_YOUNGER:
		LogOutput("karma.txt", "YOUNGER, ");
		break;

	case SAMARITAN_REL_GUILD:
		LogOutput("karma.txt", "GUILDMATE, ");
		break;

	default:
		LogOutput("karma.txt", "NO_RELATIONSHIP, ");
		break;
	}

	if (originalText)
	{
		sprintf(tempText, originalText);
		RemoveCommasAndReturnsFromString(tempText);
		LogOutput("karma.txt", tempText);
	}
	else
		LogOutput("karma.txt", "NO_TEXT");

	LogOutput("karma.txt", "\n");
}
	
//******************************************************************
void BBOSAvatar::CompareWith(BBOSAvatar *other, int &myRelationship, int &otherRelationship)
{

	int meVal = 0;
	int otherVal = 0;

	// lifeTime is in 5 minute increments

	if (charInfoArray[curCharacterIndex].lifeTime > 20)
		++meVal;
	if (charInfoArray[curCharacterIndex].lifeTime > 100)
		++meVal;
	if (charInfoArray[curCharacterIndex].lifeTime > 200)
		++meVal;
	if (charInfoArray[curCharacterIndex].lifeTime > 1000)
		++meVal;
	if (charInfoArray[curCharacterIndex].lifeTime > 3000)
		++meVal;

	if (other->charInfoArray[other->curCharacterIndex].lifeTime > 20)
		++otherVal;
	if (other->charInfoArray[other->curCharacterIndex].lifeTime > 100)
		++otherVal;
	if (other->charInfoArray[other->curCharacterIndex].lifeTime > 200)
		++otherVal;
	if (other->charInfoArray[other->curCharacterIndex].lifeTime > 1000)
		++otherVal;
	if (other->charInfoArray[other->curCharacterIndex].lifeTime > 3000)
		++otherVal;

	if (meVal == otherVal)
	{
		myRelationship    = SAMARITAN_REL_PEER;
		otherRelationship = SAMARITAN_REL_PEER;
		return;
	}
	else if (meVal > otherVal)
	{
		myRelationship    = SAMARITAN_REL_OLDER;
		otherRelationship = SAMARITAN_REL_YOUNGER;
		return;
	}

	myRelationship    = SAMARITAN_REL_YOUNGER;
	otherRelationship = SAMARITAN_REL_OLDER;

}

//******************************************************************
int BBOSAvatar::IsAGuildMate(BBOSAvatar *other)
{

	SharedSpace *sx = NULL, *sx2 = NULL;

	bboServer->FindAvatarInGuild(charInfoArray[curCharacterIndex].name, &sx);

	bboServer->FindAvatarInGuild(other->charInfoArray[other->curCharacterIndex].name, &sx2);

	if (sx != NULL && sx2 == sx)
		return TRUE;
	return FALSE;
	
	
}
	
//*******************************************************************************
// called when /tells and /replys happen
void BBOServer::HandleKarmaText(char *string, BBOSAvatar *curAvatar, BBOSAvatar *targetAv)
{

	// KARMA
	if (StringContainsThanks(string))
	{
		int myRelationship, herRelationship;

		curAvatar->CompareWith(targetAv, myRelationship, herRelationship);

		curAvatar->charInfoArray[curAvatar->curCharacterIndex].
			  karmaGiven[myRelationship][SAMARITAN_TYPE_THANKS] += 1;
		targetAv->charInfoArray[targetAv->curCharacterIndex].
			  karmaReceived[herRelationship][SAMARITAN_TYPE_THANKS] += 1;

		curAvatar->LogKarmaExchange(
					 targetAv, myRelationship, herRelationship, 
					 SAMARITAN_TYPE_THANKS, string);

		if (curAvatar->IsAGuildMate(targetAv))
		{
			curAvatar->charInfoArray[curAvatar->curCharacterIndex].
				  karmaGiven[SAMARITAN_REL_GUILD][SAMARITAN_TYPE_THANKS] += 1;
			targetAv->charInfoArray[targetAv->curCharacterIndex].
				  karmaReceived[SAMARITAN_REL_GUILD][SAMARITAN_TYPE_THANKS] += 1;

			curAvatar->LogKarmaExchange(
						 targetAv, SAMARITAN_REL_GUILD, SAMARITAN_REL_GUILD, 
						 SAMARITAN_TYPE_THANKS, string);
		}
	}
	  
	if (StringContainsWelcome(string))
	{
		int myRelationship, herRelationship;

		curAvatar->CompareWith(targetAv, myRelationship, herRelationship);

		curAvatar->charInfoArray[curAvatar->curCharacterIndex].
			  karmaGiven[myRelationship][SAMARITAN_TYPE_WELCOME] += 1;
		targetAv->charInfoArray[targetAv->curCharacterIndex].
			  karmaReceived[herRelationship][SAMARITAN_TYPE_WELCOME] += 1;

		curAvatar->LogKarmaExchange(
					 targetAv, myRelationship, herRelationship, 
					 SAMARITAN_TYPE_WELCOME, string);

		if (curAvatar->IsAGuildMate(targetAv))
		{
			curAvatar->charInfoArray[curAvatar->curCharacterIndex].
				  karmaGiven[SAMARITAN_REL_GUILD][SAMARITAN_TYPE_WELCOME] += 1;
			targetAv->charInfoArray[targetAv->curCharacterIndex].
				  karmaReceived[SAMARITAN_REL_GUILD][SAMARITAN_TYPE_WELCOME] += 1;

			curAvatar->LogKarmaExchange(
						 targetAv, SAMARITAN_REL_GUILD, SAMARITAN_REL_GUILD, 
						 SAMARITAN_TYPE_WELCOME, string);
		}
	}

	if (StringContainsPlease(string))
	{
		int myRelationship, herRelationship;

		curAvatar->CompareWith(targetAv, myRelationship, herRelationship);

		curAvatar->charInfoArray[curAvatar->curCharacterIndex].
			  karmaGiven[myRelationship][SAMARITAN_TYPE_PLEASE] += 1;
		targetAv->charInfoArray[targetAv->curCharacterIndex].
			  karmaReceived[herRelationship][SAMARITAN_TYPE_PLEASE] += 1;

		curAvatar->LogKarmaExchange(
					 targetAv, myRelationship, herRelationship, 
					 SAMARITAN_TYPE_PLEASE, string);

		if (curAvatar->IsAGuildMate(targetAv))
		{
			curAvatar->charInfoArray[curAvatar->curCharacterIndex].
				  karmaGiven[SAMARITAN_REL_GUILD][SAMARITAN_TYPE_PLEASE] += 1;
			targetAv->charInfoArray[targetAv->curCharacterIndex].
				  karmaReceived[SAMARITAN_REL_GUILD][SAMARITAN_TYPE_PLEASE] += 1;

			curAvatar->LogKarmaExchange(
						 targetAv, SAMARITAN_REL_GUILD, SAMARITAN_REL_GUILD, 
						 SAMARITAN_TYPE_PLEASE, string);
		}
	}
	// END KARMA
}

//*************************************************************************
// string matching functions

const int TY_WORD_LIST_LEN = 5;

char thanksWordList[TY_WORD_LIST_LEN][32] =
{
	{"Thank You"},
	{"Thanks"},
	{"ty"},
	{"thanx"},
	{"thnx"}
};

//*************************************************************************
int StringContainsThanks(char *string)
{
//	char tempText[1024];

	// find words
	int stringSize = 0;
	while (string[stringSize] != 0)
	{
		for (int i = 0; i < TY_WORD_LIST_LEN; ++i)
		{
			if (IsSame(&string[stringSize],thanksWordList[i]))
				return TRUE;
		}
		++stringSize;
	}

	return FALSE;
}


const int WELCOME_WORD_LIST_LEN = 7;

char welcomeWordList[WELCOME_WORD_LIST_LEN][32] =
{
	{"You're Welcome"},
	{"Youre Welcome"},
	{"Your Welcome"},
	{"yw"},
	{"No problem"},
	{"No prob"},
	{"np"}
};

//*************************************************************************
int StringContainsWelcome(char *string)
{
//	char tempText[1024];

	// find words
	int stringSize = 0;
	while (string[stringSize] != 0)
	{
		for (int i = 0; i < WELCOME_WORD_LIST_LEN; ++i)
		{
			if (IsSame(&string[stringSize],welcomeWordList[i]))
				return TRUE;
		}
		++stringSize;
	}

	return FALSE;
}


const int PLEASE_WORD_LIST_LEN = 3;

char pleaseWordList[PLEASE_WORD_LIST_LEN][32] =
{
	{"please"},
	{"pls"},
	{"plz"}
};

//*************************************************************************
int StringContainsPlease(char *string)
{
//	char tempText[1024];

	// find words
	int stringSize = 0;
	while (string[stringSize] != 0)
	{
		for (int i = 0; i < PLEASE_WORD_LIST_LEN; ++i)
		{
			if (IsSame(&string[stringSize],pleaseWordList[i]))
				return TRUE;
		}
		++stringSize;
	}

	return FALSE;
}


//*************************************************************************
void RemoveCommasAndReturnsFromString(char *string)
{

	// replace with -
	int stringSize = 0;
	while (string[stringSize] != 0)
	{
		if (',' == string[stringSize])
			string[stringSize] = '-';
		if ('\n' == string[stringSize])
			string[stringSize] = '-';

		++stringSize;
	}

}

//*******************************************************************************
int IsSame(char *a, char *b)
{
	if (!strnicmp( a , b , strlen(b)))
		return TRUE;
	return FALSE;
}

/* end of file */
