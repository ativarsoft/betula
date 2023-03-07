<?xml version="1.0" encoding="UTF-8"?>

<!DOCTYPE names [
<!ENTITY mdash "&#8212;">
]>

<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<!-- allows html entities -->
	<xsl:output method="html" encoding="utf-8" indent="yes" />
	
	<xsl:template match="bold">
		<span style="font-weight:bold;">
            		<xsl:apply-templates/>
        	</span>
	</xsl:template>

	<xsl:template match="italic">
		<span style="font-style:italic;">
			<xsl:apply-templates />
		</span>
	</xsl:template>

	<xsl:template match="hr">
		<hr style="border: 1px solid #aaa;" />
	</xsl:template>
    
	<xsl:template match="citation">
		<sup><a href="{current()}">[src]</a></sup>
	</xsl:template>
    
	<xsl:template match="/">
		<html xmlns="http://www.w3.org/1999/xhtml">
			<head>
				<meta charset="UTF-8" />
				<title>Log</title>
				<meta name="viewport" content="width=device-width, initial-scale=1.0"/>
			</head>
			<body>
				<xsl:for-each select="log/post">
					<div style="border: solid 1px #aaa; margin: 0.2em 0 0.2em 0; padding: 0.2em;">
						<p>
							<xsl:apply-templates select="body" />
						</p>
						<p style="font-size: 0.8em;">
							<xsl:text>&mdash;</xsl:text>
							<xsl:text> </xsl:text>
							<strong>
								<xsl:value-of select="author" />
							</strong>
							<xsl:text> </xsl:text>
							<span style="color: #aaa; font-style: italic;">
								<xsl:value-of select="datetime" />
							</span>
						</p>
						<p style="color: #aaa; font-size: 0.8em;">
							<xsl:text>Categories: </xsl:text>
							<xsl:for-each select="categories/tag">
								<span style="padding: 0 0.2em 0 0.2em">
									<xsl:apply-templates />
								</span>
							</xsl:for-each>
						</p>
					</div>
				</xsl:for-each>
			</body>
		</html>
	</xsl:template>
</xsl:stylesheet>
