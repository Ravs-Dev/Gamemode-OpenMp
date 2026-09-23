
stock AccountCheck(playerid)
{
    g_MysqlRaceCheck[playerid]++;
    printf("RACE CHECK %d", g_MysqlRaceCheck[playerid]);

    GetPlayerName(playerid, pData[playerid][pUCP], MAX_PLAYER_NAME);

	new query[128];
	mysql_format(g_SQL, query, sizeof(query), "SELECT * FROM ucp WHERE ucp_name = '%e' LIMIT 1", pData[playerid][pUCP]);
	mysql_tquery(g_SQL, query, "OnUCPChecked", "dd", playerid, g_MysqlRaceCheck[playerid]);
    return 1;
}

Function::OnUCPChecked(playerid, race_check)
{
	if(race_check != g_MysqlRaceCheck[playerid]) return KickEx(playerid);

	new string[180];
	if(cache_num_rows() > 0)
	{
        cache_get_value_name_int(0, "verifycode", pData[playerid][pVerifyCode]);
        cache_get_value_name_int(0, "activation", pData[playerid][pActivation]);
		cache_get_value_name(0, "password", pData[playerid][pPassword]);

        // printf("kode ucp chked %d", pData[playerid][pVerifyCode]);
        // printf("pw chked %s", pData[playerid][pPassword]);

        if(pData[playerid][pActivation] == 0)
        {
            format(string, sizeof(string), "{ffffff}Username: {04d400}%s\n{ffffff} Status: {04d400}Terdaftar\n{ffffff}Akun UCP kamu sudah terdaftar, silahkan masukan kode verifikasi.", pData[playerid][pUCP]);
            Dialog_Show(playerid, DialogVerifyCode, DIALOG_STYLE_INPUT, "Verifikasi UCP", string, "Input", "Close");
        }
        else
        {
            format(string, sizeof(string), "{ffffff}Username: {04d400}%s\n{ffffff}Status: {04d400}Terdaftar\n{ffffff}Akun UCP kamu sudah terdaftar kedalam server.\nLogin ke akun kamu dengan memasukan password dibawah:", pData[playerid][pUCP]);
            Dialog_Show(playerid, DialogLogin, DIALOG_STYLE_PASSWORD, "Login", string, "Login", "Close");
        }

	}
	else
	{
		format(string, sizeof(string), "{ffffff}Username: {04d400}%s\n{ffffff} Status: {bd0000}Tidak terdaftar\n{ffffff}Akun UCP kamu belum terdaftar didalam server.\nSilahkan kunjungi discord server untuk membuat akun UCP.", pData[playerid][pUCP]);
		Dialog_Show(playerid, DialogUnused, DIALOG_STYLE_MSGBOX, "UCP", string, "Close", "");
        KickEx(playerid);
	}
	return 1;
}

Function::OnPasswordHashed(playerid, hash_id)
{
	bcrypt_get_hash(pData[playerid][pPassword], BCRYPT_HASH_LENGTH);
    pData[playerid][pActivation] = 1;
    // printf("pw chked %s", pData[playerid][pPassword]);
    
    new query[250];
    mysql_format(g_SQL, query, sizeof(query), "UPDATE ucp set activation = '%d', password = '%e' WHERE ucp_name = '%e'", pData[playerid][pActivation], pData[playerid][pPassword], pData[playerid][pUCP]);
    mysql_tquery(g_SQL, query);
	return 1;
}


Function::OnPasswordVerify(playerid, bool:success)
{
    new str[185];
	if(success)
	{
        CheckUserCharacter(playerid);
	}
	else
	{
		pData[playerid][pLoginAttempt]++;

		if(pData[playerid][pLoginAttempt] >= 3)
		{
            format(str, sizeof(str), "{ffffff}Username: {04d400}%s\n{ffffff}Status: {04d400}Terdaftar\n{ffffff}Kamu dikick karena salah memasukan password sebanyak 3X!\n{FBFF00}Lupa password? Ajukan reset password pada moderator di discord kammi", pData[playerid][pUCP]);
			Dialog_Show(playerid, DialogUnused, DIALOG_STYLE_MSGBOX, "Failed Login", str, "Oke", "");
			KickEx(playerid);
		}
		else
        {
            format(str, sizeof(str), "{ffffff}Username: {04d400}%s\n{ffffff}Status: {04d400}Terdaftar\n{FBFF00}Kamu salah memasukan password! kesempatan kamu %d/3.\n{ffffff}Isi kembali kolom password dengan benar:", pData[playerid][pUCP], pData[playerid][pLoginAttempt]);
            Dialog_Show(playerid, DialogLogin, DIALOG_STYLE_PASSWORD, "Login", str, "Input", "Close");
        }
	}
	return 1;
}

stock CheckUserCharacter(playerid)
{
    g_MysqlRaceCheck[playerid]++;

    new query[128];
    mysql_format(g_SQL, query, sizeof(query), "SELECT username FROM player WHERE ucp = '%s' LIMIT %d", pData[playerid][pUCP], MAX_CHARS);
    mysql_tquery(g_SQL, query, "OnCharacterLoaded", "dd", playerid, g_MysqlRaceCheck[playerid]);
    return 1;
}

Function::OnCharacterLoaded(playerid, race_check)
{
    if(race_check != g_MysqlRaceCheck[playerid]) return KickEx(playerid);

    new rows = cache_num_rows();

    Forex(i, MAX_CHARS)
    {
        PlayerCharacter[playerid][i][0] = EOS;
        print("eos");
    }

    Forex(k, rows)
    {
        cache_get_value_name(k, "username", PlayerCharacter[playerid][k]);
        print("name");
    }
    ShowCharacterList(playerid);
    return 1;
}

stock ShowCharacterList(playerid)
{
    new name[268],
        str[128],
        count;

    for(new i; i < MAX_CHARS; i++)
    {
        if(PlayerCharacter[playerid][i][0] != EOS)
        {
            format(str, sizeof(str), "%s\n", PlayerCharacter[playerid][i]);
            strcat(name, str);
            count++;
        }
    }

    if(count < MAX_CHARS) strcat(name, "+ New Character");
    
    Dialog_Show(playerid, DialogListChar, DIALOG_STYLE_LIST, "List Character", name, "Choose", "Close");
    return 1;
}

Function::OnUsernameChecked(playerid, const name[], race_check)
{
    if(race_check != g_MysqlRaceCheck[playerid])    
        return KickEx(playerid);
    
    new rows = cache_num_rows();
    if(rows)
        return Dialog_Show(playerid, DialogCreateChar, DIALOG_STYLE_INPUT, "Create New Character", "{bd0000}Nama sudah pernah dipakai! {ffffff}Gunakan nama lain\nMasukan nama :", "Input", "");

    Dialog_Show(playerid, DialogAge, DIALOG_STYLE_INPUT, "Age", "Masukan tanggal lahir dengan format DD/MM/YYYY (16/01/2005)", "Input", "Tutup");
    return 1;
}

Function::OnInsertCharacter(playerid)
{
    pData[playerid][pID] = cache_insert_id();

    SetPlayerName(playerid, pData[playerid][pName]);

    pData[playerid][isLogin] = true;
    CreateSetupCharacter(playerid);
    return 1;
}

Function::OnCharacterDataLoaded(playerid)
{
    CalculateTickCount(playerid, "Load Character");

	cache_get_value_name_int(0, "id", pData[playerid][pID]);
    printf("ID %d", pData[playerid][pID]);
    if(pData[playerid][pID] < 1)
    {
        SendError(playerid, "Akun ini tidak terdaftar dalam database");
        KickEx(playerid);
        return 1;
    }

	cache_get_value_name_float(0, "pos_x", pData[playerid][pPos][0]);
	cache_get_value_name_float(0, "pos_y", pData[playerid][pPos][1]);
	cache_get_value_name_float(0, "pos_z", pData[playerid][pPos][2]);
	cache_get_value_name_float(0, "pos_a", pData[playerid][pPos][3]);

	cache_get_value_name_int(0, "interior", pData[playerid][pInterior]);
	cache_get_value_name_int(0, "virtual_world", pData[playerid][pVirtual_World]);
	cache_get_value_name_int(0, "skin", pData[playerid][pSkin]);
	cache_get_value_name_int(0, "money", pData[playerid][pMoney]);
	cache_get_value_name_int(0, "level", pData[playerid][pLevel]);

    pData[playerid][isLogin] = true;
    SetSpawnInfo(playerid, NO_TEAM, pData[playerid][pSkin], pData[playerid][pPos][0], pData[playerid][pPos][1], pData[playerid][pPos][2], pData[playerid][pPos][3], WEAPON_FIST, 0, WEAPON_FIST, 0, WEAPON_FIST, 0);
    SpawnPlayer(playerid);
    GetPlayerNameEx(playerid);
    
    SetPlayerInterior(playerid, pData[playerid][pInterior]);
    SetPlayerVirtualWorld(playerid, pData[playerid][pVirtual_World]);
    GivePlayerMoney(playerid, pData[playerid][pMoney]);
    SetPlayerScore(playerid, pData[playerid][pLevel]);
	return 1;
}