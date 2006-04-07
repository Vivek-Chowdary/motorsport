<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="/">
    <xsl:for-each select="scene/nodes">
        <xsl:apply-templates select="node"/>
    </xsl:for-each>
</xsl:template>

<xsl:template match="node">
    <xsl:choose>
        <xsl:when test="entity">
            <xsl:apply-templates select="entity"/>
        </xsl:when>
        <xsl:when test="camera">
            <xsl:apply-templates select="camera"/>
        </xsl:when>
        <xsl:when test="userData/property/@name = 'type'">
            <xsl:apply-templates select="userData"/>
        </xsl:when>
        <xsl:otherwise>
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

<xsl:template match="camera">
    <camera>
        <xsl:attribute name="name">
            <xsl:value-of select="../@name"/>
        </xsl:attribute>
        <xsl:attribute name="position">
            <xsl:value-of select="../position/@x"/><xsl:text> </xsl:text>
            <xsl:value-of select="../position/@y"/><xsl:text> </xsl:text>
            <xsl:value-of select="../position/@z"/>
        </xsl:attribute>
        <xsl:attribute name="rotation">
            <xsl:value-of select="../rotation/@qw"/><xsl:text> </xsl:text>
            <xsl:value-of select="../rotation/@qx"/><xsl:text> </xsl:text>
            <xsl:value-of select="../rotation/@qy"/><xsl:text> </xsl:text>
            <xsl:value-of select="../rotation/@qz"/>
        </xsl:attribute>
        <xsl:attribute name="scale">
            <xsl:value-of select="../scale/@x"/><xsl:text> </xsl:text>
            <xsl:value-of select="../scale/@y"/><xsl:text> </xsl:text>
            <xsl:value-of select="../scale/@z"/>
        </xsl:attribute>
        <xsl:for-each select="../userData/property">
            <xsl:if test="@name = 'target'">
                <xsl:attribute name="target"><xsl:value-of select="@data"/></xsl:attribute>
            </xsl:if>
        </xsl:for-each>
        <xsl:attribute name="projectionType"><xsl:value-of select="@projectionType"/></xsl:attribute>
        <xsl:attribute name="fov"><xsl:value-of select="@fov"/></xsl:attribute>
        <xsl:attribute name="normal"><xsl:apply-templates select="normal"/></xsl:attribute>
        <xsl:attribute name="clipping"><xsl:apply-templates select="clipping"/></xsl:attribute>
    </camera>
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
        <xsl:if test="@name = 'type'">
            <xsl:element name="{@data}">
                <xsl:for-each select="../..">
                    <xsl:attribute name="name"><xsl:value-of select="@name"/></xsl:attribute>
                    <xsl:attribute name="position">
                        <xsl:value-of select="position/@x"/><xsl:text> </xsl:text>
                        <xsl:value-of select="position/@y"/><xsl:text> </xsl:text>
                        <xsl:value-of select="position/@z"/>
                    </xsl:attribute>
                    <xsl:attribute name="rotation">
                        <xsl:value-of select="rotation/@qw"/><xsl:text> </xsl:text>
                        <xsl:value-of select="rotation/@qx"/><xsl:text> </xsl:text>
                        <xsl:value-of select="rotation/@qy"/><xsl:text> </xsl:text>
                        <xsl:value-of select="rotation/@qz"/>
                    </xsl:attribute>
                    <xsl:attribute name="scale">
                        <xsl:value-of select="scale/@x"/><xsl:text> </xsl:text>
                        <xsl:value-of select="scale/@y"/><xsl:text> </xsl:text>
                        <xsl:value-of select="scale/@z"/>
                    </xsl:attribute>
                </xsl:for-each>
                <xsl:for-each select="../property">
                    <xsl:if test="@name != 'type'">
                        <xsl:attribute name="{@name}">
                            <xsl:value-of select="@data"/>
                        </xsl:attribute>
                    </xsl:if>
                </xsl:for-each>
                <xsl:for-each select="../..">
                    <xsl:apply-templates select="node"/>
                </xsl:for-each>
            </xsl:element>
        </xsl:if>
    </xsl:for-each>
</xsl:template>

<xsl:template match="entity">
    <mesh>
        <xsl:attribute name="name">
            <xsl:value-of select="@name"/>
        </xsl:attribute>
        <xsl:attribute name="position">
            <xsl:value-of select="../position/@x"/><xsl:text> </xsl:text>
            <xsl:value-of select="../position/@y"/><xsl:text> </xsl:text>
            <xsl:value-of select="../position/@z"/>
        </xsl:attribute>
        <xsl:attribute name="rotation">
            <xsl:value-of select="../rotation/@qw"/><xsl:text> </xsl:text>
            <xsl:value-of select="../rotation/@qx"/><xsl:text> </xsl:text>
            <xsl:value-of select="../rotation/@qy"/><xsl:text> </xsl:text>
            <xsl:value-of select="../rotation/@qz"/>
        </xsl:attribute>
        <xsl:attribute name="scale">
            <xsl:value-of select="../scale/@x"/><xsl:text> </xsl:text>
            <xsl:value-of select="../scale/@y"/><xsl:text> </xsl:text>
            <xsl:value-of select="../scale/@z"/>
        </xsl:attribute>
        <xsl:attribute name="file">
            <xsl:value-of select="@meshFile"/>
        </xsl:attribute>
    </mesh>
</xsl:template>

</xsl:stylesheet>
