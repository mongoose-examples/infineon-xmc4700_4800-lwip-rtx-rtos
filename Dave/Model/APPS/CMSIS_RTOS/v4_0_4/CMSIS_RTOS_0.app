<?xml version="1.0" encoding="ASCII"?>
<ResourceModel:App xmi:version="2.0" xmlns:xmi="http://www.omg.org/XMI" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ResourceModel="http://www.infineon.com/Davex/Resource.ecore" name="CMSIS_RTOS" URI="http://resources/4.0.4/app/CMSIS_RTOS/0" description="The CMSIS_RTOS APP provides an adapter layer on top of specific RTOS &#xA;implantations to provide a standard CMSIS RTOS API." version="4.0.4" minDaveVersion="4.1.2" instanceLabel="CMSIS_RTOS_0" appLabel="">
  <upwardMapList xsi:type="ResourceModel:RequiredApp" href="../../ETH_LWIP/v4_1_4/ETH_LWIP_0.app#//@requiredApps.1"/>
  <properties singleton="true" provideInit="true" sharable="true"/>
  <requiredApps URI="http://resources/4.0.4/app/CMSIS_RTOS/0/ra_cmsis_rtos_impl" requiredAppName="CMSIS_RTOS_RTX" requiringMode="SHARABLE">
    <downwardMapList xsi:type="ResourceModel:App" href="../../CMSIS_RTOS_RTX/v4_0_20/CMSIS_RTOS_RTX_0.app#/"/>
  </requiredApps>
</ResourceModel:App>
