<?xml version="1.0" encoding="ASCII"?>
<ResourceModel:App xmi:version="2.0" xmlns:xmi="http://www.omg.org/XMI" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ResourceModel="http://www.infineon.com/Davex/Resource.ecore" name="HTTP_SERVER" URI="http://resources/4.0.12/app/HTTP_SERVER/0" description="HTTP server is used to store, process and deliver web pages to clients&#xA;using ethernet interface." mode="NOTSHARABLE" version="4.0.12" minDaveVersion="4.1.2" instanceLabel="HTTP_SERVER_0" appLabel="">
  <properties singleton="true" provideInit="true"/>
  <requiredApps URI="http://resources/4.0.12/app/HTTP_SERVER/0/appres_lwip" requiredAppName="ETH_LWIP" requiringMode="SHARABLE">
    <downwardMapList xsi:type="ResourceModel:App" href="../../ETH_LWIP/v4_1_4/ETH_LWIP_0.app#/"/>
  </requiredApps>
  <requiredApps URI="http://resources/4.0.12/app/HTTP_SERVER/0/appres_fatfs" requiredAppName="FATFS" required="false" requiringMode="SHARABLE"/>
</ResourceModel:App>
