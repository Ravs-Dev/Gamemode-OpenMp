stock AddVeh(playerid, model, Float:x, Float:y, Float:z, Float:a, color1, color2)
{
    new vehicleid = CreateVehicle(model, x, y, z, a, color1, color2, -1, false);
    if(vehicleid == INVALID_VEHICLE_ID) return 0;
    
    pvData[vehicleid][vOwner] = pData[playerid][pID];
    pvData[vehicleid][vModel] = model;
    pvData[vehicleid][vPos][0] = x;
    pvData[vehicleid][vPos][1] = y;
    pvData[vehicleid][vPos][2] = z;
    pvData[vehicleid][vPos][3] = a;
    pvData[vehicleid][vColor1] = color1;
    pvData[vehicleid][vColor2] = color2;
    pvData[vehicleid][isExists] = true;

    Iter_Add(PlayerVehicle, vehicleid);

    new query[450];
    mysql_format(g_SQL, query, sizeof(query), "INSERT INTO vehicles (owner_id, model, pos_x, pos_y, pos_z, pos_a, color1, color2) VALUES ('%d', '%d', '%f', '%f', '%f', '%f', '%d', '%d')", 
    pData[playerid][pID], model, x, y, z, a, color1, color2);
    mysql_tquery(g_SQL, query, "OnVehicleCreated", "d", vehicleid);
    return 1;
}

stock UpdateDataVehicle(vehicleid)
{
    if(vehicleid == INVALID_VEHICLE_ID) return 0;
    if(!pvData[vehicleid][isExists]) return 0;
    if(pvData[vehicleid][vID] <= 0) return 0;

    new query[300];
    mysql_format(g_SQL, query, sizeof(query), "UPDATE vehicles SET pos_x = '%f', pos_y = '%f', pos_z = '%f', pos_a = '%f' WHERE id = '%d' LIMIT 1", 
    pvData[vehicleid][vPos][0], pvData[vehicleid][vPos][1], pvData[vehicleid][vPos][2], 
    pvData[vehicleid][vPos][3], pvData[vehicleid][vID]);
    
    mysql_tquery(g_SQL, query);
    printf("[VEHICLE] Kendaraan id %d diupdate!", pvData[vehicleid][vID]);
    return 1;
}

forward OnVehicleLoaded();
public OnVehicleLoaded()
{
    new rows = cache_num_rows();
    if(!rows)
    {
        print("[VEHICLE] Tidak ada kendaraan yang diload!");
        return 1;
    }
    
    new loaded;
    for(new i = 0; i < rows; i++)
    { 
        new model, owner_id, vehicle_id,
            Float:x, Float:y, Float:z, Float:a,
            color1, color2;
        
        cache_get_value_name_int(i, "id", vehicle_id);
        cache_get_value_name_int(i, "owner_id", owner_id);
        cache_get_value_name_int(i, "model", model);

        cache_get_value_name_float(i, "pos_x", x);
        cache_get_value_name_float(i, "pos_y", y);
        cache_get_value_name_float(i, "pos_z", z);
        cache_get_value_name_float(i, "pos_a", a);

        cache_get_value_name_int(i, "color1", color1);
        cache_get_value_name_int(i, "color2", color2);

        new vehicleid = CreateVehicle(model, x, y, z, a, color1, color2, -1, false);
        if(vehicleid == INVALID_VEHICLE_ID) continue;

        pvData[vehicleid][vID] = vehicle_id;
        pvData[vehicleid][vOwner] = owner_id;
        pvData[vehicleid][vModel] = model;
        pvData[vehicleid][vPos][0] = x;
        pvData[vehicleid][vPos][1] = y;
        pvData[vehicleid][vPos][2] = z;
        pvData[vehicleid][vPos][3] = a;
        pvData[vehicleid][vColor1] = color1;
        pvData[vehicleid][vColor2] = color2;
        pvData[vehicleid][isExists] = true;

        Iter_Add(PlayerVehicle, vehicleid);
        loaded++;
    }
    printf("[VEHICLE] Kendaraan sebanyak %d berhasil dimuat", loaded);
    return 1;
}

forward OnVehicleCreated(vehicleid);
public OnVehicleCreated(vehicleid)
{
    new insert_id = cache_insert_id();
    if(vehicleid != INVALID_VEHICLE_ID && insert_id > 0)
    {
        pvData[vehicleid][vID] = insert_id;
    }
    printf("[VEHICLE] Sebuah kendaraan berhasil dibuat dengan id = %d",  pvData[vehicleid][vID]);
    return 1;
}

