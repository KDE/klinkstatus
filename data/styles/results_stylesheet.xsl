<?xml version='1.0' encoding='utf-8' ?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    
    <xsl:output method="html" />
    
    <xsl:template match="/klinkstatus">
        <html>
            <body>
                <table border="1">
                    <thead>
                        <tr>
                            <th>URL</th>
                            <th>Status</th>
                            <th>Label</th>
                            <th>Referrers</th>
                        </tr>
                    </thead>
                    <tbody>
                        <xsl:for-each select="/klinkstatus/link_list/link">                  
                            <tr>
                                <td>         
                                    <xsl:apply-templates select="url"/> 
                                </td>            
                                <td>         
                                    <xsl:apply-templates select="status"/>
                                </td>            
                                <td>         
                                    <xsl:apply-templates select="label"/>
                                </td>            
                                <td>         
                                    <xsl:apply-templates select="referrers"/>
                                </td>            
                            </tr>         
                        </xsl:for-each>
                    </tbody>                  
                </table>                    
            </body>
        </html>
    </xsl:template>
    
    
    <xsl:template match="url">
        <xsl:value-of select="."/>
    </xsl:template>
    
    <xsl:template match="status">
        <xsl:value-of select="."/>
        <br/>      
    </xsl:template>
    
    <xsl:template match="label">      
        <xsl:value-of select="."/>
        <br/>      
    </xsl:template>
    
    <xsl:template match="referrers">
        <xsl:for-each select="url">
            <xsl:apply-templates select="."/>
        </xsl:for-each>   
    </xsl:template>
    
    
    <xsl:template match="/klinkstatus/link_list/link/referrers/url">
        <xsl:value-of select="."/>
        <br/>
    </xsl:template>
    
</xsl:stylesheet>
