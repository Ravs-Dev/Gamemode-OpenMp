stock ShowStats(playerid)
{
    new str[155], 
        strhead[64],
        client[8];
    if(!pData[playerid][isOfficialClient])
        format(client, sizeof(client), "Android");
    else
        format(client, sizeof(client), "PC");
    
    format(strhead, sizeof(strhead), "%s|[%d]", pData[playerid][pName], playerid);
    format(str, sizeof(str), "{ffffff}Client\t:\t%s\nID_Database\t:\t%d\nInterior\t:\t%d\nVirtual World\t:\t%d\nSkin\t:\t%d\nMoney\t:\t%d\nLevel\t:\t%d", 
    client, pData[playerid][pID], 
    pData[playerid][pInterior], pData[playerid][pVirtual_World], 
    pData[playerid][pSkin], pData[playerid][pMoney],
    pData[playerid][pLevel]);

    return Dialog_Show(playerid, DialogUnused, DIALOG_STYLE_MSGBOX, strhead, str, "Oke", "");
}

stock ParseBirthDate(playerid, const date[])
{
    new day,
        month,
        year;

    if(sscanf(date, "p</>ddd", day, month, year))
    {
        Dialog_Show(playerid, DialogAge, DIALOG_STYLE_INPUT, "Age", "{ffffff}Kamu memasukan format yang {B82B2B}salah!\n{ffffff}Masukan kembali tanggal lahir dengan format DD/MM/YYYY (16/01/2005)", "Input", "Tutup");
        return 0;
    }

    if(!IsValidDate(day, month, year))
    {
        Dialog_Show(playerid, DialogAge, DIALOG_STYLE_INPUT, "Age", "{ffffff}Kamu memasukan tanggal yang {B82B2B}tidak valid!\n{ffffff}Masukan kembali tanggal lahir dengan format DD/MM/YYYY dan valid!", "Input", "Tutup");
        return 0;
    }

    pData[playerid][pBirthDay] = day;
    pData[playerid][pBirthMonth] = month;
    pData[playerid][pBirthYear] = year;
    printf("Player %s tanggal lahir: %d/%d/%d", pData[playerid][pName], pData[playerid][pBirthDay], pData[playerid][pBirthMonth], pData[playerid][pBirthYear]);
    Dialog_Show(playerid, DialogGender, DIALOG_STYLE_LIST, "Gender", "Male\nFemale", "Continue", "");
    return 1;
}

stock IsValidDate(day, month, year)
{
    if(year < 1900 || year > 2100) return 0;
    if(month < 1 || month > 12) return 0;
    if(day < 1) return 0;

    new daysInMonth[] = {
    31,28,31,30,31,30,31,31,30,31,30,31};

    return day <= daysInMonth[month - 1];
}

stock IsValidRPName(const name[])
{
    new len = strlen(name);

    if(len < 3 || len > MAX_PLAYER_NAME)
        return 0;

    new underscorePos = strfind(name, "_");
    if(underscorePos == -1)
        return 0;

    if(underscorePos == 0 || underscorePos == len - 1)
        return 0;

    if(name[0] < 'A' || name[0] > 'Z')
        return 0;

    if(name[underscorePos + 1] < 'A' || name[underscorePos + 1] > 'Z')
        return 0;

    for(new i = 0; i < len; i++)
    {
        if(name[i] == '_') continue;

        if(name[i] < 'a' || name[i] > 'z')
        {
            if(name[i] < 'A' || name[i] > 'Z')
                return 0;
        }
    }
    return 1;
}

CreateSetupCharacter(playerid)
{
    if(!pData[playerid][isLogin]) return SendError(playerid, "Kamu tidak memiliki akes untuk melakukan ini!");

    SetSpawnInfo(playerid, NO_TEAM, pData[playerid][pSkin], DEFAULT_POS_X, DEFAULT_POS_Y, DEFAULT_POS_Z, DEFAULT_POS_A, WEAPON_FIST, 0, WEAPON_FIST, 0, WEAPON_FIST, 0);
    GivePlayerMoneyEx(playerid, 5000);
    SpawnPlayer(playerid);
    return 1;
}

stock UpdateDataPlayer(playerid)
{
	if(!pData[playerid][isLogin]) return 1;

	GetPlayerPos(playerid, pData[playerid][pPos][0], pData[playerid][pPos][1], pData[playerid][pPos][2]);
	GetPlayerFacingAngle(playerid, pData[playerid][pPos][3]);
	pData[playerid][pInterior] = GetPlayerInterior(playerid);
	pData[playerid][pVirtual_World] = GetPlayerVirtualWorld(playerid);
	pData[playerid][pSkin] = GetPlayerSkin(playerid);
	pData[playerid][pMoney] = GetPlayerMoney(playerid);
	pData[playerid][pLevel] = GetPlayerScore(playerid);

	new query[500];
	mysql_format(g_SQL, query, sizeof(query), "UPDATE player SET pos_x = %f, pos_y = %f, pos_z = %f, pos_a = %f, interior = %d, virtual_world = %d, skin = %d, money = %d, level = %d WHERE id = %d LIMIT 1",
	pData[playerid][pPos][0], pData[playerid][pPos][1], pData[playerid][pPos][2], 
	pData[playerid][pPos][3], pData[playerid][pInterior], pData[playerid][pVirtual_World], 
	pData[playerid][pSkin], pData[playerid][pMoney], pData[playerid][pLevel],
	pData[playerid][pID]);

	mysql_tquery(g_SQL, query);
	printf("[MYSQL] Data player id %d berhasil disimpan", pData[playerid][pID]);
	return 1;
}

stock GetPlayerNameEx(playerid)
{   
    GetPlayerName(playerid, pData[playerid][pName], MAX_PLAYER_NAME);
    return pData[playerid][pName];
}

ResetValueVariable(playerid)
{
    pData[playerid][pID] = -1;
    pData[playerid][pName] = EOS;
    pData[playerid][pSkin] = 0;
    pData[playerid][pMoney] = 0;
    pData[playerid][pLevel] = 0;
    pData[playerid][pPos][0] = 0.0;
    pData[playerid][pPos][1] = 0.0;
    pData[playerid][pPos][2] = 0.0;
    pData[playerid][pPos][3] = 0.0;
    pData[playerid][pBirthDay] = 0;
    pData[playerid][pBirthMonth] = 0;
    pData[playerid][pBirthYear] = 0;
    pData[playerid][pGender] = -1;

    // bagian login register
    pData[playerid][isLogin] = false;
    pData[playerid][pLoginAttempt] = 0;
    pData[playerid][isOfficialClient] = false;
    pData[playerid][pVerifyCode] = 0;
    pData[playerid][pActivation] = 0;

    print("[PLAYER] Variable berhasil direset");
    return 1;
}

GivePlayerMoneyEx(playerid, ammount)
{
    printf("Uang sebelum ditambah %d", pData[playerid][pMoney]);
    GivePlayerMoney(playerid, ammount);
    pData[playerid][pMoney] += ammount;
    printf("Uang berhasil ditambah %d", pData[playerid][pMoney]);
    return 1;
}