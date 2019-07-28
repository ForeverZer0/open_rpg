module OpenRPG

  ##
  # Provides methods to create pre-defined {Color} instances for convenience.
  module Colors
    
		##
		# @return [Color] `Color.new(0, 0, 0, 0)`
		#
		# <div style="background-color:#000000 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.none
			Color.new(0, 0, 0, 0)
		end

		##
		# @return [Color] `Color.new(255, 255, 255, 0)`
		#
		# <div style="background-color:#ffffff ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.transparent
			Color.new(255, 255, 255, 0)
		end

		##
		# @return [Color] `Color.new(240, 248, 255, 255)`
		#
		# <div style="background-color:#f0f8ff ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.alice_blue
			Color.new(240, 248, 255, 255)
		end

		##
		# @return [Color] `Color.new(250, 235, 215, 255)`
		#
		# <div style="background-color:#faebd7 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.antique_white
			Color.new(250, 235, 215, 255)
		end

		##
		# @return [Color] `Color.new(0, 255, 255, 255)`
		#
		# <div style="background-color:#00ffff ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.aqua
			Color.new(0, 255, 255, 255)
		end

		##
		# @return [Color] `Color.new(127, 255, 212, 255)`
		#
		# <div style="background-color:#7fffd4 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.aquamarine
			Color.new(127, 255, 212, 255)
		end

		##
		# @return [Color] `Color.new(240, 255, 255, 255)`
		#
		# <div style="background-color:#f0ffff ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.azure
			Color.new(240, 255, 255, 255)
		end

		##
		# @return [Color] `Color.new(245, 245, 220, 255)`
		#
		# <div style="background-color:#f5f5dc ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.beige
			Color.new(245, 245, 220, 255)
		end

		##
		# @return [Color] `Color.new(255, 228, 196, 255)`
		#
		# <div style="background-color:#ffe4c4 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.bisque
			Color.new(255, 228, 196, 255)
		end

		##
		# @return [Color] `Color.new(0, 0, 0, 255)`
		#
		# <div style="background-color:#000000 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.black
			Color.new(0, 0, 0, 255)
		end

		##
		# @return [Color] `Color.new(255, 235, 205, 255)`
		#
		# <div style="background-color:#ffebcd ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.blanched_almond
			Color.new(255, 235, 205, 255)
		end

		##
		# @return [Color] `Color.new(0, 0, 255, 255)`
		#
		# <div style="background-color:#0000ff ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.blue
			Color.new(0, 0, 255, 255)
		end

		##
		# @return [Color] `Color.new(138, 43, 226, 255)`
		#
		# <div style="background-color:#8a2be2 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.blue_violet
			Color.new(138, 43, 226, 255)
		end

		##
		# @return [Color] `Color.new(165, 42, 42, 255)`
		#
		# <div style="background-color:#a52a2a ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.brown
			Color.new(165, 42, 42, 255)
		end

		##
		# @return [Color] `Color.new(222, 184, 135, 255)`
		#
		# <div style="background-color:#deb887 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.burly_wood
			Color.new(222, 184, 135, 255)
		end

		##
		# @return [Color] `Color.new(95, 158, 160, 255)`
		#
		# <div style="background-color:#5f9ea0 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.cadet_blue
			Color.new(95, 158, 160, 255)
		end

		##
		# @return [Color] `Color.new(127, 255, 0, 255)`
		#
		# <div style="background-color:#7fff00 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.chartreuse
			Color.new(127, 255, 0, 255)
		end

		##
		# @return [Color] `Color.new(210, 105, 30, 255)`
		#
		# <div style="background-color:#d2691e ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.chocolate
			Color.new(210, 105, 30, 255)
		end

		##
		# @return [Color] `Color.new(255, 127, 80, 255)`
		#
		# <div style="background-color:#ff7f50 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.coral
			Color.new(255, 127, 80, 255)
		end

		##
		# @return [Color] `Color.new(100, 149, 237, 255)`
		#
		# <div style="background-color:#6495ed ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.cornflower_blue
			Color.new(100, 149, 237, 255)
		end

		##
		# @return [Color] `Color.new(255, 248, 220, 255)`
		#
		# <div style="background-color:#fff8dc ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.cornsilk
			Color.new(255, 248, 220, 255)
		end

		##
		# @return [Color] `Color.new(220, 20, 60, 255)`
		#
		# <div style="background-color:#dc143c ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.crimson
			Color.new(220, 20, 60, 255)
		end

		##
		# @return [Color] `Color.new(0, 255, 255, 255)`
		#
		# <div style="background-color:#00ffff ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.cyan
			Color.new(0, 255, 255, 255)
		end

		##
		# @return [Color] `Color.new(0, 0, 139, 255)`
		#
		# <div style="background-color:#00008b ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.dark_blue
			Color.new(0, 0, 139, 255)
		end

		##
		# @return [Color] `Color.new(0, 139, 139, 255)`
		#
		# <div style="background-color:#008b8b ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.dark_cyan
			Color.new(0, 139, 139, 255)
		end

		##
		# @return [Color] `Color.new(184, 134, 11, 255)`
		#
		# <div style="background-color:#b8860b ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.dark_goldenrod
			Color.new(184, 134, 11, 255)
		end

		##
		# @return [Color] `Color.new(169, 169, 169, 255)`
		#
		# <div style="background-color:#a9a9a9 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.dark_gray
			Color.new(169, 169, 169, 255)
		end

		##
		# @return [Color] `Color.new(0, 100, 0, 255)`
		#
		# <div style="background-color:#006400 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.dark_green
			Color.new(0, 100, 0, 255)
		end

		##
		# @return [Color] `Color.new(189, 183, 107, 255)`
		#
		# <div style="background-color:#bdb76b ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.dark_khaki
			Color.new(189, 183, 107, 255)
		end

		##
		# @return [Color] `Color.new(139, 0, 139, 255)`
		#
		# <div style="background-color:#8b008b ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.dark_magenta
			Color.new(139, 0, 139, 255)
		end

		##
		# @return [Color] `Color.new(85, 107, 47, 255)`
		#
		# <div style="background-color:#556b2f ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.dark_olive_green
			Color.new(85, 107, 47, 255)
		end

		##
		# @return [Color] `Color.new(255, 140, 0, 255)`
		#
		# <div style="background-color:#ff8c00 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.dark_orange
			Color.new(255, 140, 0, 255)
		end

		##
		# @return [Color] `Color.new(153, 50, 204, 255)`
		#
		# <div style="background-color:#9932cc ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.dark_orchid
			Color.new(153, 50, 204, 255)
		end

		##
		# @return [Color] `Color.new(139, 0, 0, 255)`
		#
		# <div style="background-color:#8b0000 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.dark_red
			Color.new(139, 0, 0, 255)
		end

		##
		# @return [Color] `Color.new(233, 150, 122, 255)`
		#
		# <div style="background-color:#e9967a ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.dark_salmon
			Color.new(233, 150, 122, 255)
		end

		##
		# @return [Color] `Color.new(143, 188, 139, 255)`
		#
		# <div style="background-color:#8fbc8b ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.dark_sea_green
			Color.new(143, 188, 139, 255)
		end

		##
		# @return [Color] `Color.new(72, 61, 139, 255)`
		#
		# <div style="background-color:#483d8b ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.dark_slate_blue
			Color.new(72, 61, 139, 255)
		end

		##
		# @return [Color] `Color.new(47, 79, 79, 255)`
		#
		# <div style="background-color:#2f4f4f ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.dark_slate_gray
			Color.new(47, 79, 79, 255)
		end

		##
		# @return [Color] `Color.new(0, 206, 209, 255)`
		#
		# <div style="background-color:#00ced1 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.dark_turquoise
			Color.new(0, 206, 209, 255)
		end

		##
		# @return [Color] `Color.new(148, 0, 211, 255)`
		#
		# <div style="background-color:#9400d3 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.dark_violet
			Color.new(148, 0, 211, 255)
		end

		##
		# @return [Color] `Color.new(255, 20, 147, 255)`
		#
		# <div style="background-color:#ff1493 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.deep_pink
			Color.new(255, 20, 147, 255)
		end

		##
		# @return [Color] `Color.new(0, 191, 255, 255)`
		#
		# <div style="background-color:#00bfff ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.deep_sky_blue
			Color.new(0, 191, 255, 255)
		end

		##
		# @return [Color] `Color.new(105, 105, 105, 255)`
		#
		# <div style="background-color:#696969 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.dim_gray
			Color.new(105, 105, 105, 255)
		end

		##
		# @return [Color] `Color.new(30, 144, 255, 255)`
		#
		# <div style="background-color:#1e90ff ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.dodger_blue
			Color.new(30, 144, 255, 255)
		end

		##
		# @return [Color] `Color.new(178, 34, 34, 255)`
		#
		# <div style="background-color:#b22222 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.firebrick
			Color.new(178, 34, 34, 255)
		end

		##
		# @return [Color] `Color.new(255, 250, 240, 255)`
		#
		# <div style="background-color:#fffaf0 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.floral_white
			Color.new(255, 250, 240, 255)
		end

		##
		# @return [Color] `Color.new(34, 139, 34, 255)`
		#
		# <div style="background-color:#228b22 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.forest_green
			Color.new(34, 139, 34, 255)
		end

		##
		# @return [Color] `Color.new(255, 0, 255, 255)`
		#
		# <div style="background-color:#ff00ff ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.fuchsia
			Color.new(255, 0, 255, 255)
		end

		##
		# @return [Color] `Color.new(220, 220, 220, 255)`
		#
		# <div style="background-color:#dcdcdc ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.gainsboro
			Color.new(220, 220, 220, 255)
		end

		##
		# @return [Color] `Color.new(248, 248, 255, 255)`
		#
		# <div style="background-color:#f8f8ff ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.ghost_white
			Color.new(248, 248, 255, 255)
		end

		##
		# @return [Color] `Color.new(255, 215, 0, 255)`
		#
		# <div style="background-color:#ffd700 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.gold
			Color.new(255, 215, 0, 255)
		end

		##
		# @return [Color] `Color.new(218, 165, 32, 255)`
		#
		# <div style="background-color:#daa520 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.goldenrod
			Color.new(218, 165, 32, 255)
		end

		##
		# @return [Color] `Color.new(128, 128, 128, 255)`
		#
		# <div style="background-color:#808080 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.gray
			Color.new(128, 128, 128, 255)
		end

		##
		# @return [Color] `Color.new(0, 128, 0, 255)`
		#
		# <div style="background-color:#008000 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.green
			Color.new(0, 128, 0, 255)
		end

		##
		# @return [Color] `Color.new(173, 255, 47, 255)`
		#
		# <div style="background-color:#adff2f ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.green_yellow
			Color.new(173, 255, 47, 255)
		end

		##
		# @return [Color] `Color.new(240, 255, 240, 255)`
		#
		# <div style="background-color:#f0fff0 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.honeydew
			Color.new(240, 255, 240, 255)
		end

		##
		# @return [Color] `Color.new(255, 105, 180, 255)`
		#
		# <div style="background-color:#ff69b4 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.hot_pink
			Color.new(255, 105, 180, 255)
		end

		##
		# @return [Color] `Color.new(205, 92, 92, 255)`
		#
		# <div style="background-color:#cd5c5c ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.indian_red
			Color.new(205, 92, 92, 255)
		end

		##
		# @return [Color] `Color.new(75, 0, 130, 255)`
		#
		# <div style="background-color:#4b0082 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.indigo
			Color.new(75, 0, 130, 255)
		end

		##
		# @return [Color] `Color.new(255, 255, 240, 255)`
		#
		# <div style="background-color:#fffff0 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.ivory
			Color.new(255, 255, 240, 255)
		end

		##
		# @return [Color] `Color.new(240, 230, 140, 255)`
		#
		# <div style="background-color:#f0e68c ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.khaki
			Color.new(240, 230, 140, 255)
		end

		##
		# @return [Color] `Color.new(230, 230, 250, 255)`
		#
		# <div style="background-color:#e6e6fa ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.lavender
			Color.new(230, 230, 250, 255)
		end

		##
		# @return [Color] `Color.new(255, 240, 245, 255)`
		#
		# <div style="background-color:#fff0f5 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.lavender_blush
			Color.new(255, 240, 245, 255)
		end

		##
		# @return [Color] `Color.new(124, 252, 0, 255)`
		#
		# <div style="background-color:#7cfc00 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.lawn_green
			Color.new(124, 252, 0, 255)
		end

		##
		# @return [Color] `Color.new(255, 250, 205, 255)`
		#
		# <div style="background-color:#fffacd ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.lemon_chiffon
			Color.new(255, 250, 205, 255)
		end

		##
		# @return [Color] `Color.new(173, 216, 230, 255)`
		#
		# <div style="background-color:#add8e6 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.light_blue
			Color.new(173, 216, 230, 255)
		end

		##
		# @return [Color] `Color.new(240, 128, 128, 255)`
		#
		# <div style="background-color:#f08080 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.light_coral
			Color.new(240, 128, 128, 255)
		end

		##
		# @return [Color] `Color.new(224, 255, 255, 255)`
		#
		# <div style="background-color:#e0ffff ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.light_cyan
			Color.new(224, 255, 255, 255)
		end

		##
		# @return [Color] `Color.new(250, 250, 210, 255)`
		#
		# <div style="background-color:#fafad2 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.light_goldenrod_yellow
			Color.new(250, 250, 210, 255)
		end

		##
		# @return [Color] `Color.new(211, 211, 211, 255)`
		#
		# <div style="background-color:#d3d3d3 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.light_gray
			Color.new(211, 211, 211, 255)
		end

		##
		# @return [Color] `Color.new(144, 238, 144, 255)`
		#
		# <div style="background-color:#90ee90 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.light_green
			Color.new(144, 238, 144, 255)
		end

		##
		# @return [Color] `Color.new(255, 182, 193, 255)`
		#
		# <div style="background-color:#ffb6c1 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.light_pink
			Color.new(255, 182, 193, 255)
		end

		##
		# @return [Color] `Color.new(255, 160, 122, 255)`
		#
		# <div style="background-color:#ffa07a ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.light_salmon
			Color.new(255, 160, 122, 255)
		end

		##
		# @return [Color] `Color.new(32, 178, 170, 255)`
		#
		# <div style="background-color:#20b2aa ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.light_sea_green
			Color.new(32, 178, 170, 255)
		end

		##
		# @return [Color] `Color.new(135, 206, 250, 255)`
		#
		# <div style="background-color:#87cefa ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.light_sky_blue
			Color.new(135, 206, 250, 255)
		end

		##
		# @return [Color] `Color.new(119, 136, 153, 255)`
		#
		# <div style="background-color:#778899 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.light_slate_gray
			Color.new(119, 136, 153, 255)
		end

		##
		# @return [Color] `Color.new(176, 196, 222, 255)`
		#
		# <div style="background-color:#b0c4de ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.light_steel_blue
			Color.new(176, 196, 222, 255)
		end

		##
		# @return [Color] `Color.new(255, 255, 224, 255)`
		#
		# <div style="background-color:#ffffe0 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.light_yellow
			Color.new(255, 255, 224, 255)
		end

		##
		# @return [Color] `Color.new(0, 255, 0, 255)`
		#
		# <div style="background-color:#00ff00 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.lime
			Color.new(0, 255, 0, 255)
		end

		##
		# @return [Color] `Color.new(50, 205, 50, 255)`
		#
		# <div style="background-color:#32cd32 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.lime_green
			Color.new(50, 205, 50, 255)
		end

		##
		# @return [Color] `Color.new(250, 240, 230, 255)`
		#
		# <div style="background-color:#faf0e6 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.linen
			Color.new(250, 240, 230, 255)
		end

		##
		# @return [Color] `Color.new(255, 0, 255, 255)`
		#
		# <div style="background-color:#ff00ff ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.magenta
			Color.new(255, 0, 255, 255)
		end

		##
		# @return [Color] `Color.new(128, 0, 0, 255)`
		#
		# <div style="background-color:#800000 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.maroon
			Color.new(128, 0, 0, 255)
		end

		##
		# @return [Color] `Color.new(102, 205, 170, 255)`
		#
		# <div style="background-color:#66cdaa ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.medium_aquamarine
			Color.new(102, 205, 170, 255)
		end

		##
		# @return [Color] `Color.new(0, 0, 205, 255)`
		#
		# <div style="background-color:#0000cd ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.medium_blue
			Color.new(0, 0, 205, 255)
		end

		##
		# @return [Color] `Color.new(186, 85, 211, 255)`
		#
		# <div style="background-color:#ba55d3 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.medium_orchid
			Color.new(186, 85, 211, 255)
		end

		##
		# @return [Color] `Color.new(147, 112, 219, 255)`
		#
		# <div style="background-color:#9370db ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.medium_purple
			Color.new(147, 112, 219, 255)
		end

		##
		# @return [Color] `Color.new(60, 179, 113, 255)`
		#
		# <div style="background-color:#3cb371 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.medium_sea_green
			Color.new(60, 179, 113, 255)
		end

		##
		# @return [Color] `Color.new(123, 104, 238, 255)`
		#
		# <div style="background-color:#7b68ee ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.medium_slate_blue
			Color.new(123, 104, 238, 255)
		end

		##
		# @return [Color] `Color.new(0, 250, 154, 255)`
		#
		# <div style="background-color:#00fa9a ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.medium_spring_green
			Color.new(0, 250, 154, 255)
		end

		##
		# @return [Color] `Color.new(72, 209, 204, 255)`
		#
		# <div style="background-color:#48d1cc ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.medium_turquoise
			Color.new(72, 209, 204, 255)
		end

		##
		# @return [Color] `Color.new(199, 21, 133, 255)`
		#
		# <div style="background-color:#c71585 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.medium_violet_red
			Color.new(199, 21, 133, 255)
		end

		##
		# @return [Color] `Color.new(25, 25, 112, 255)`
		#
		# <div style="background-color:#191970 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.midnight_blue
			Color.new(25, 25, 112, 255)
		end

		##
		# @return [Color] `Color.new(245, 255, 250, 255)`
		#
		# <div style="background-color:#f5fffa ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.mint_cream
			Color.new(245, 255, 250, 255)
		end

		##
		# @return [Color] `Color.new(255, 228, 225, 255)`
		#
		# <div style="background-color:#ffe4e1 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.misty_rose
			Color.new(255, 228, 225, 255)
		end

		##
		# @return [Color] `Color.new(255, 228, 181, 255)`
		#
		# <div style="background-color:#ffe4b5 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.moccasin
			Color.new(255, 228, 181, 255)
		end

		##
		# @return [Color] `Color.new(255, 222, 173, 255)`
		#
		# <div style="background-color:#ffdead ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.navajo_white
			Color.new(255, 222, 173, 255)
		end

		##
		# @return [Color] `Color.new(0, 0, 128, 255)`
		#
		# <div style="background-color:#000080 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.navy
			Color.new(0, 0, 128, 255)
		end

		##
		# @return [Color] `Color.new(253, 245, 230, 255)`
		#
		# <div style="background-color:#fdf5e6 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.old_lace
			Color.new(253, 245, 230, 255)
		end

		##
		# @return [Color] `Color.new(128, 128, 0, 255)`
		#
		# <div style="background-color:#808000 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.olive
			Color.new(128, 128, 0, 255)
		end

		##
		# @return [Color] `Color.new(107, 142, 35, 255)`
		#
		# <div style="background-color:#6b8e23 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.olive_drab
			Color.new(107, 142, 35, 255)
		end

		##
		# @return [Color] `Color.new(255, 165, 0, 255)`
		#
		# <div style="background-color:#ffa500 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.orange
			Color.new(255, 165, 0, 255)
		end

		##
		# @return [Color] `Color.new(255, 69, 0, 255)`
		#
		# <div style="background-color:#ff4500 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.orange_red
			Color.new(255, 69, 0, 255)
		end

		##
		# @return [Color] `Color.new(218, 112, 214, 255)`
		#
		# <div style="background-color:#da70d6 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.orchid
			Color.new(218, 112, 214, 255)
		end

		##
		# @return [Color] `Color.new(238, 232, 170, 255)`
		#
		# <div style="background-color:#eee8aa ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.pale_goldenrod
			Color.new(238, 232, 170, 255)
		end

		##
		# @return [Color] `Color.new(152, 251, 152, 255)`
		#
		# <div style="background-color:#98fb98 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.pale_green
			Color.new(152, 251, 152, 255)
		end

		##
		# @return [Color] `Color.new(175, 238, 238, 255)`
		#
		# <div style="background-color:#afeeee ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.pale_turquoise
			Color.new(175, 238, 238, 255)
		end

		##
		# @return [Color] `Color.new(219, 112, 147, 255)`
		#
		# <div style="background-color:#db7093 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.pale_violet_red
			Color.new(219, 112, 147, 255)
		end

		##
		# @return [Color] `Color.new(255, 239, 213, 255)`
		#
		# <div style="background-color:#ffefd5 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.papaya_whip
			Color.new(255, 239, 213, 255)
		end

		##
		# @return [Color] `Color.new(255, 218, 185, 255)`
		#
		# <div style="background-color:#ffdab9 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.peach_puff
			Color.new(255, 218, 185, 255)
		end

		##
		# @return [Color] `Color.new(205, 133, 63, 255)`
		#
		# <div style="background-color:#cd853f ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.peru
			Color.new(205, 133, 63, 255)
		end

		##
		# @return [Color] `Color.new(255, 192, 203, 255)`
		#
		# <div style="background-color:#ffc0cb ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.pink
			Color.new(255, 192, 203, 255)
		end

		##
		# @return [Color] `Color.new(221, 160, 221, 255)`
		#
		# <div style="background-color:#dda0dd ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.plum
			Color.new(221, 160, 221, 255)
		end

		##
		# @return [Color] `Color.new(176, 224, 230, 255)`
		#
		# <div style="background-color:#b0e0e6 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.powder_blue
			Color.new(176, 224, 230, 255)
		end

		##
		# @return [Color] `Color.new(128, 0, 128, 255)`
		#
		# <div style="background-color:#800080 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.purple
			Color.new(128, 0, 128, 255)
		end

		##
		# @return [Color] `Color.new(255, 0, 0, 255)`
		#
		# <div style="background-color:#ff0000 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.red
			Color.new(255, 0, 0, 255)
		end

		##
		# @return [Color] `Color.new(188, 143, 143, 255)`
		#
		# <div style="background-color:#bc8f8f ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.rosy_brown
			Color.new(188, 143, 143, 255)
		end

		##
		# @return [Color] `Color.new(65, 105, 225, 255)`
		#
		# <div style="background-color:#4169e1 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.royal_blue
			Color.new(65, 105, 225, 255)
		end

		##
		# @return [Color] `Color.new(139, 69, 19, 255)`
		#
		# <div style="background-color:#8b4513 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.saddle_brown
			Color.new(139, 69, 19, 255)
		end

		##
		# @return [Color] `Color.new(250, 128, 114, 255)`
		#
		# <div style="background-color:#fa8072 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.salmon
			Color.new(250, 128, 114, 255)
		end

		##
		# @return [Color] `Color.new(244, 164, 96, 255)`
		#
		# <div style="background-color:#f4a460 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.sandy_brown
			Color.new(244, 164, 96, 255)
		end

		##
		# @return [Color] `Color.new(46, 139, 87, 255)`
		#
		# <div style="background-color:#2e8b57 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.sea_green
			Color.new(46, 139, 87, 255)
		end

		##
		# @return [Color] `Color.new(255, 245, 238, 255)`
		#
		# <div style="background-color:#fff5ee ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.sea_shell
			Color.new(255, 245, 238, 255)
		end

		##
		# @return [Color] `Color.new(160, 82, 45, 255)`
		#
		# <div style="background-color:#a0522d ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.sienna
			Color.new(160, 82, 45, 255)
		end

		##
		# @return [Color] `Color.new(192, 192, 192, 255)`
		#
		# <div style="background-color:#c0c0c0 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.silver
			Color.new(192, 192, 192, 255)
		end

		##
		# @return [Color] `Color.new(135, 206, 235, 255)`
		#
		# <div style="background-color:#87ceeb ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.sky_blue
			Color.new(135, 206, 235, 255)
		end

		##
		# @return [Color] `Color.new(106, 90, 205, 255)`
		#
		# <div style="background-color:#6a5acd ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.slate_blue
			Color.new(106, 90, 205, 255)
		end

		##
		# @return [Color] `Color.new(112, 128, 144, 255)`
		#
		# <div style="background-color:#708090 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.slate_gray
			Color.new(112, 128, 144, 255)
		end

		##
		# @return [Color] `Color.new(255, 250, 250, 255)`
		#
		# <div style="background-color:#fffafa ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.snow
			Color.new(255, 250, 250, 255)
		end

		##
		# @return [Color] `Color.new(0, 255, 127, 255)`
		#
		# <div style="background-color:#00ff7f ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.spring_green
			Color.new(0, 255, 127, 255)
		end

		##
		# @return [Color] `Color.new(70, 130, 180, 255)`
		#
		# <div style="background-color:#4682b4 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.steel_blue
			Color.new(70, 130, 180, 255)
		end

		##
		# @return [Color] `Color.new(210, 180, 140, 255)`
		#
		# <div style="background-color:#d2b48c ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.tan
			Color.new(210, 180, 140, 255)
		end

		##
		# @return [Color] `Color.new(0, 128, 128, 255)`
		#
		# <div style="background-color:#008080 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.teal
			Color.new(0, 128, 128, 255)
		end

		##
		# @return [Color] `Color.new(216, 191, 216, 255)`
		#
		# <div style="background-color:#d8bfd8 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.thistle
			Color.new(216, 191, 216, 255)
		end

		##
		# @return [Color] `Color.new(255, 99, 71, 255)`
		#
		# <div style="background-color:#ff6347 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.tomato
			Color.new(255, 99, 71, 255)
		end

		##
		# @return [Color] `Color.new(64, 224, 208, 255)`
		#
		# <div style="background-color:#40e0d0 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.turquoise
			Color.new(64, 224, 208, 255)
		end

		##
		# @return [Color] `Color.new(238, 130, 238, 255)`
		#
		# <div style="background-color:#ee82ee ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.violet
			Color.new(238, 130, 238, 255)
		end

		##
		# @return [Color] `Color.new(245, 222, 179, 255)`
		#
		# <div style="background-color:#f5deb3 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.wheat
			Color.new(245, 222, 179, 255)
		end

		##
		# @return [Color] `Color.new(255, 255, 255, 255)`
		#
		# <div style="background-color:#ffffff ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.white
			Color.new(255, 255, 255, 255)
		end

		##
		# @return [Color] `Color.new(245, 245, 245, 255)`
		#
		# <div style="background-color:#f5f5f5 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.white_smoke
			Color.new(245, 245, 245, 255)
		end

		##
		# @return [Color] `Color.new(255, 255, 0, 255)`
		#
		# <div style="background-color:#ffff00 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.yellow
			Color.new(255, 255, 0, 255)
		end

		##
		# @return [Color] `Color.new(154, 205, 50, 255)`
		#
		# <div style="background-color:#9acd32 ;width:24px;height:24px;border:1px solid #000;"></div>
		def self.yellow_green
			Color.new(154, 205, 50, 255)
    end
	end
end
