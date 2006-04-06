<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:template match="/">
<vehicle>
	<xsl:for-each select="scene/nodes/node">
<xsl:if test="@name = 'vehicle'">
		<xsl:apply-templates select="userData"/>
		<xsl:for-each select="node">
<xsl:if test="@name = 'body'">
			<body>
				<xsl:apply-templates select="userData"/>
				<xsl:for-each select="node"><xsl:sort select="@name" order="ascending"/><xsl:if test="entity">
					<child>
						<xsl:attribute name="name"><xsl:value-of select="@name"/></xsl:attribute>
						<xsl:attribute name="mesh"><xsl:apply-templates select="entity"/></xsl:attribute>
						<xsl:attribute name="rotation"><xsl:apply-templates select="rotation"/></xsl:attribute>
						<xsl:attribute name="position"><xsl:apply-templates select="position"/></xsl:attribute>
						<xsl:if test="userData"><xsl:apply-templates select="userData"/></xsl:if>
					</child>
				</xsl:if></xsl:for-each>
			</body>
		</xsl:if></xsl:for-each>
		<xsl:for-each select="node">
<xsl:sort select="@name" order="ascending"/><xsl:if test="camera">
			<camera>
				<xsl:attribute name="name"><xsl:value-of select="@name"/></xsl:attribute>
				<xsl:attribute name="position"><xsl:apply-templates select="position"/></xsl:attribute>
				<xsl:attribute name="rotation"><xsl:apply-templates select="rotation"/></xsl:attribute>
				<xsl:apply-templates select="camera"/>
			</camera>
		</xsl:if></xsl:for-each>
		<xsl:for-each select="node"><xsl:if test="@name = 'engine'">
			<engine>
				<xsl:attribute name="name"><xsl:value-of select="@name"/></xsl:attribute>
				<xsl:attribute name="position"><xsl:apply-templates select="position"/></xsl:attribute>
				<xsl:attribute name="rotation"><xsl:apply-templates select="rotation"/></xsl:attribute>
		                <xsl:for-each select="userData">
				<xsl:attribute name="angularVelLimit"><xsl:value-of select="@angularVelLimit"/></xsl:attribute>
				<xsl:attribute name="engineFriction"><xsl:value-of select="@engineFriction"/></xsl:attribute>
				<xsl:attribute name="engineInertia"><xsl:value-of select="@engineInertia"/></xsl:attribute>
				<xsl:attribute name="torqueLinearMultiplier"><xsl:value-of select="@torqueLinearMultiplier"/></xsl:attribute>
                                </xsl:for-each>
			</engine>
		</xsl:if></xsl:for-each>
		<xsl:for-each select="node"><xsl:if test="@name = 'clutch'">
			<clutch>
				<xsl:attribute name="name"><xsl:value-of select="@name"/></xsl:attribute>
				<xsl:attribute name="position"><xsl:apply-templates select="position"/></xsl:attribute>
				<xsl:attribute name="rotation"><xsl:apply-templates select="rotation"/></xsl:attribute>
				<xsl:attribute name="coeffDynamicFriction"><xsl:value-of select="@coeffDynamicFriction"/></xsl:attribute>
				<xsl:attribute name="coeffStaticFriction"><xsl:value-of select="@coeffStaticFriction"/></xsl:attribute>
				<xsl:attribute name="maxTorqueTransfer"><xsl:value-of select="@maxTorqueTransfer"/></xsl:attribute>
			</clutch>
		</xsl:if></xsl:for-each>
		
		<xsl:for-each select="node"><xsl:if test="@name = 'gearbox'">
			<gearbox>
				<xsl:attribute name="name"><xsl:value-of select="@name"/></xsl:attribute>
				<xsl:attribute name="position"><xsl:apply-templates select="position"/></xsl:attribute>
				<xsl:attribute name="rotation"><xsl:apply-templates select="rotation"/></xsl:attribute>
				<xsl:attribute name="gearboxFriction"><xsl:value-of select="@gearboxFriction"/></xsl:attribute>
				<xsl:attribute name="gearboxInertia"><xsl:value-of select="@gearboxInertia"/></xsl:attribute>
				<xsl:for-each select="node"><xsl:if test="@name = 'gear'">
					<gear>
						<xsl:attribute name="name"><xsl:value-of select="@name"/></xsl:attribute>
						<xsl:attribute name="rotation"><xsl:apply-templates select="rotation"/></xsl:attribute>
						<xsl:attribute name="position"><xsl:apply-templates select="position"/></xsl:attribute>
						<xsl:attribute name="number"><xsl:value-of select="@number"/></xsl:attribute>
						<xsl:attribute name="ratio"><xsl:value-of select="@ratio"/></xsl:attribute>
					</gear>
				</xsl:if></xsl:for-each>
			</gearbox>
		</xsl:if></xsl:for-each>
		<xsl:for-each select="node"><xsl:if test="@name = 'finalDrive'">
			<finalDrive>
				<xsl:attribute name="name"><xsl:value-of select="@name"/></xsl:attribute>
				<xsl:attribute name="position"><xsl:apply-templates select="position"/></xsl:attribute>
				<xsl:attribute name="rotation"><xsl:apply-templates select="rotation"/></xsl:attribute>
				<xsl:attribute name="diffFriction"><xsl:value-of select="@diffFriction"/></xsl:attribute>
				<xsl:attribute name="diffInertia"><xsl:value-of select="@diffInertia"/></xsl:attribute>
				<xsl:attribute name="finalDriveRatio"><xsl:value-of select="@finalDriveRatio"/></xsl:attribute>
			</finalDrive>
		</xsl:if></xsl:for-each>
						
		<!--  TODO in mosp
		<lights>
			<xsl:attribute name="empty">dummy</xsl:attribute>
			<xsl:for-each select="node">
<xsl:sort select="@name" order="ascending"/>
<xsl:if test="light">
				Name = <xsl:value-of select="@name"/><br />
				PosRot = <xsl:apply-templates select="position"/>, <xsl:apply-templates select="rotation"/><br />
				Data: <xsl:apply-templates select="light"/>
			</xsl:if></xsl:for-each>
		</lights>
		-->			
	</xsl:if></xsl:for-each>
</vehicle>
</xsl:template>

<xsl:template match="node">
</xsl:template>

<xsl:template match="light">
                <xsl:value-of select="@type"/>
        </xsl:template>
        <xsl:template match="camera">
                <xsl:attribute name="fov">
                        <xsl:value-of select="@fov"/>
                </xsl:attribute>
                <xsl:attribute name="normal">
                        <xsl:apply-templates select="normal"/>
                </xsl:attribute>
                <xsl:attribute name="clipping">
                        <xsl:apply-templates select="clipping"/>
                </xsl:attribute>
                <xsl:attribute name="projectionType">
                        <xsl:value-of select="@projectionType"/>
                </xsl:attribute>
        </xsl:template>
        <xsl:template match="normal">
                <xsl:value-of select="@x"/><xsl:text> </xsl:text>
                <xsl:value-of select="@y"/><xsl:text> </xsl:text>
                <xsl:value-of select="@z"/>
        </xsl:template>
        <xsl:template match="clipping">
                <xsl:value-of select="@near"/><xsl:text> </xsl:text>
                <xsl:value-of select="@far"/>
        </xsl:template>
        <xsl:template match="userData">
                <xsl:for-each select="property">
                        <xsl:attribute name="{@name}">
                                <xsl:value-of select="@data"/>
                        </xsl:attribute>
                </xsl:for-each>
        </xsl:template>
        <xsl:template match="entity">
                <xsl:value-of select="@meshFile"/>
        </xsl:template>
        <xsl:template match="position">
                <xsl:value-of select="@x"/><xsl:text> </xsl:text>
                <xsl:value-of select="@y"/><xsl:text> </xsl:text>
                <xsl:value-of select="@z"/>
        </xsl:template>
        <xsl:template match="rotation">
                <xsl:value-of select="@qw"/><xsl:text> </xsl:text>
                <xsl:value-of select="@qx"/><xsl:text> </xsl:text>
                <xsl:value-of select="@qy"/><xsl:text> </xsl:text>
                <xsl:value-of select="@qz"/>
        </xsl:template>
</xsl:stylesheet>
