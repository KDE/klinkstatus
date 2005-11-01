<?xml version='1.0' encoding='utf-8' ?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    
    <xsl:output method="html" />
    
    <xsl:template match="/klinkstatus">
        <html>
            <body>
                
                <strong>URL: </strong>
                <xsl:value-of select="/klinkstatus/url"/>
                <br/>            
                <strong>Depth: </strong>
                <xsl:value-of select="/klinkstatus/depth"/>            
                <br/>            
                <strong>Check Parent Folders: </strong>
                <xsl:value-of select="/klinkstatus/check_parent_folders"/>            
                <br/>            
                <strong>Check external links: </strong>
                <xsl:value-of select="/klinkstatus/check_external_links"/>            
                <br/>                            
                <xsl:if test="/klinkstatus/check_regular_expression[@check = 'true']">
                    <strong>Do not check links matching regular expression: </strong>
                    <xsl:value-of select="/klinkstatus/check_regular_expression"/>                    
                    <br/>            
                </xsl:if>               
                <br/>            
                
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
        <xsl:variable name="isBroken">
            <xsl:value-of select="../status/@broken"/>
        </xsl:variable>
        <xsl:choose>
            <xsl:when test="$isBroken='true'">
                <span style="color: red;"> 
                    <xsl:value-of select="."/>
                </span> 
            </xsl:when>
            <xsl:otherwise>              
                <xsl:value-of select="."/>
            </xsl:otherwise>
        </xsl:choose>      
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
