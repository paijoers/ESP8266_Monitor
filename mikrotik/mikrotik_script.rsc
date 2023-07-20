# Tidak kompatibel untuk RouterOS 7
# Silahkan edit sesuaikan dengan dengan settingan mikrotik masing-masing 

/system scheduler
add name="closed" interval=1d start-time="03:00:00" on-event={
# eksekusi script saat cafe atau warkop tutup. contoh :
/ip dhcp-server set lease-time=00:10:00 [find name=hotspot];
/ip hotspot
profile set html-directory=cafe/closed login-by="http-chap" [find name=hotspot1];
cookie remove [find where user=guest];
active remove [find where user=guest];
host remove [find server=hotspot1 !bypassed];
}

add name="open" interval=1d start-time="06:00:00" on-event={
# eksekusi script saat cafe atau warkop buka. contoh :
/ip dhcp-server set lease-time=02:00:00 [find name=hotspot];
/ip hotspot profile set html-directory=cafe/open login-by="http-chap,mac-cookie" [find name=hotspot1];
/file set cafe/open/closed.txt contents=[/system scheduler get closed start-time];
}

add name="ESP Monitor" interval=10s on-event={
# mencari otomatis ip address ESP8266 dari dhcp lease
:local hostname "ESP Monitor";
:local espIP;
:foreach lease in=[/ip dhcp-server lease find where host-name=$hostname] do={
 :set espIP [/ip dhcp-server lease get $lease address];
}

# mengambil data mikrotik contoh dibawah ini mengambil value identify, password user guest, waktu closed dan jumlah user hotspot yang aktif
:local var1 [/system identity get name];
:local var2 [/ip hotspot user get guest password];
:local var3 [/system scheduler get closed start-time];
:local var4 [/ip hotspot active print count-only];

# membuat objek key/value json dan dikirim ke ESP8266 untuk ditampilkan ke LCD
:local json "{\"data1\":\"$var1\",\"data2\":\"$var2\",\"data3\":\"$var3\",\"data4\":\"$var4\"}";
/tool fetch mode=http url=("http://$espIP/update") http-method=post output=none http-data=$json;
}
