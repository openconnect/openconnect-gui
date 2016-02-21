// vpnc-script-win.js
//
// Sets up the Network interface and the routes
// needed by vpnc.

// --------------------------------------------------------------
// Utilities
// --------------------------------------------------------------

var fs = WScript.CreateObject("Scripting.FileSystemObject");
var tmpdir = fs.GetSpecialFolder(2)+"\\";
var log = fs.OpenTextFile(tmpdir + "vpnc.log", 8, true);

function echo(msg)
{
       log.WriteLine(msg);
}

function exec(cmd)
 {
       var s = "";
       log.WriteLine("executing: " + cmd);

       if (fs.FileExists(tmpdir + "vpnc.out")) {
               fs.DeleteFile(tmpdir + "vpnc.out");
       }
       ws.Run("cmd.exe /c " +cmd+" > " + tmpdir + "vpnc.out", 0, true);

       if (fs.FileExists(tmpdir + "vpnc.out")) {
               var f = fs.OpenTextFile(tmpdir + "vpnc.out", 1);
               if (f) {
                      if (!f.AtEndOfStream) {
                           s = f.ReadAll();
                       }
                       log.Write(s);
                       f.Close();
	       }
       }
       return s;
}
  
function getDefaultGateway()
{
	if (exec("route print").match(/0\.0\.0\.0 *(0|128)\.0\.0\.0 *([0-9\.]*)/)) {
		return (RegExp.$2);
	}
	return ("");
}

function getInterfaceId(ifname)
{
	var rx_ifid = new RegExp("^ *([0-9]+) *.*" + ifname + "$", "m")
	if (exec("netsh interface ip show interfaces").search(rx_ifid)) {
		return (RegExp.$1);
	}
	return ("");
}

function waitForInterface() {
	var if_route = new RegExp(env("INTERNAL_IP4_ADDRESS") + " *255.255.255.255");
	for (var i = 0; i < 5; i++) {
		echo("Waiting for interface to come up...");
		WScript.Sleep(2000);
		if (exec("route print").match(if_route)) {
			return true;
		}
	}
	return false;
}

// --------------------------------------------------------------
// Script starts here
// --------------------------------------------------------------

var internal_ip4_netmask = "255.255.255.0";

var ws = WScript.CreateObject("WScript.Shell");
var env = ws.Environment("Process");

// How to add the default internal route
// 0 - As interface gateway when setting properties
// 1 - As a 0.0.0.0/0 route with a lower metric than the default route
// 2 - As 0.0.0.0/1 + 128.0.0.0/1 routes (override the default route cleanly)
var REDIRECT_GATEWAY_METHOD = 0;

switch (env("reason")) {
case "pre-init":
	break;
case "connect":
	var gw = getDefaultGateway();
	var address_array = env("INTERNAL_IP4_ADDRESS").split(".");
	var netmask_array = env("INTERNAL_IP4_NETMASK").split(".");
	// Calculate the first usable address in subnet
	var internal_gw_array = new Array(
		address_array[0] & netmask_array[0],
		address_array[1] & netmask_array[1],
		address_array[2] & netmask_array[2],
		(address_array[3] & netmask_array[3]) + 1
	);
	var internal_gw = internal_gw_array.join(".");
	var tundevid = getInterfaceId(env("TUNDEV"))
	
	echo("VPN Gateway: " + env("VPNGATEWAY"));
	echo("Internal Address: " + env("INTERNAL_IP4_ADDRESS"));
	echo("Internal Netmask: " + env("INTERNAL_IP4_NETMASK"));
	echo("Internal Gateway: " + internal_gw);
	echo("Interface: \"" + tundevid + "\"");

	// Add direct route for the VPN gateway to avoid routing loops
	exec("route add " + env("VPNGATEWAY") +
            " mask 255.255.255.255 " + gw);

	if (env("INTERNAL_IP4_MTU")) {
	    echo("MTU: " + env("INTERNAL_IP4_MTU"));
	    exec("netsh interface ipv4 set subinterface \"" + tundevid +
		"\" mtu=" + env("INTERNAL_IP4_MTU") + " store=active");
	    if (env("INTERNAL_IP6_ADDRESS")) {
		exec("netsh interface ipv6 set subinterface \"" + tundevid +
		    "\" mtu=" + env("INTERNAL_IP4_MTU") + " store=active");
	    }
	}

	echo("Configuring \"" + tundevid + "\" interface for Legacy IP...");
	
	if (!env("CISCO_SPLIT_INC") && REDIRECT_GATEWAY_METHOD != 2) {
		// Interface metric must be set to 1 in order to add a route with metric 1 since Windows Vista
		exec("netsh interface ip set interface \"" + tundevid + "\" metric=1");
	}
	
	if (env("CISCO_SPLIT_INC") || REDIRECT_GATEWAY_METHOD > 0) {
		exec("netsh interface ip set address \"" + tundevid + "\" static " +
			env("INTERNAL_IP4_ADDRESS") + " " + env("INTERNAL_IP4_NETMASK"));
	} else {
		// The default route will be added automatically
		exec("netsh interface ip set address \"" + tundevid + "\" static " +
			env("INTERNAL_IP4_ADDRESS") + " " + env("INTERNAL_IP4_NETMASK") + " " + internal_gw + " 1");
	}

    if (env("INTERNAL_IP4_NBNS")) {
		var wins = env("INTERNAL_IP4_NBNS").split(/ /);
		for (var i = 0; i < wins.length; i++) {
	                exec("netsh interface ip add wins \"" +
			    tundevid + "\" " + wins[i]
			    + " index=" + (i+1));
		}
	}

    if (env("INTERNAL_IP4_DNS")) {
		var dns = env("INTERNAL_IP4_DNS").split(/ /);
		for (var i = 0; i < dns.length; i++) {
	                exec("netsh interface ip add dns \"" +
			    tundevid + "\" " + dns[i]
			    + " index=" + (i+1));
		}
	}
	echo("done.");

	// Add internal network routes
    echo("Configuring Legacy IP networks:");
    if (env("CISCO_SPLIT_INC")) {
		// Waiting for the interface to be configured before to add routes
		if (!waitForInterface()) {
			echo("Interface does not seem to be up.");
		}
		
		for (var i = 0 ; i < parseInt(env("CISCO_SPLIT_INC")); i++) {
			var network = env("CISCO_SPLIT_INC_" + i + "_ADDR");
			var netmask = env("CISCO_SPLIT_INC_" + i + "_MASK");
			var netmasklen = env("CISCO_SPLIT_INC_" + i +
					 "_MASKLEN");
			exec("route add " + network + " mask " + netmask +
			     " " + internal_gw);
		}
	} else if (REDIRECT_GATEWAY_METHOD > 0) {
		// Waiting for the interface to be configured before to add routes
		if (!waitForInterface()) {
			echo("Interface does not seem to be up.");
		}
		
		if (REDIRECT_GATEWAY_METHOD == 1) {
			exec("route add 0.0.0.0 mask 0.0.0.0 " + internal_gw + " metric 1");
		} else {
			exec("route add 0.0.0.0 mask 128.0.0.0 " + internal_gw);
			exec("route add 128.0.0.0 mask 128.0.0.0 " + internal_gw);
		}
	}
	echo("Route configuration done.");

        if (env("INTERNAL_IP6_ADDRESS")) {
		echo("Configuring \"" + tundevid + "\" interface for IPv6...");

		exec("netsh interface ipv6 set address \"" + tundevid + "\" " +
		    env("INTERNAL_IP6_ADDRESS") + " store=active");

		echo("done.");

		// Add internal network routes
	        echo("Configuring Legacy IP networks:");
	        if (env("INTERNAL_IP6_NETMASK") && !env("INTERNAL_IP6_NETMASK").match("/128$")) {
			exec("netsh interface ipv6 add route " + env("INTERNAL_IP6_NETMASK") +
			    " \"" + tundevid + "\" fe80::8 store=active");
		}

	        if (env("CISCO_IPV6_SPLIT_INC")) {
			for (var i = 0 ; i < parseInt(env("CISCO_IPV6_SPLIT_INC")); i++) {
				var network = env("CISCO_IPV6_SPLIT_INC_" + i + "_ADDR");
				var netmasklen = env("CISCO_SPLIT_INC_" + i +
						 "_MASKLEN");
				exec("netsh interface ipv6 add route " + network + "/" +
				    netmasklen + " \"" + tundevid + "\" fe80::8 store=active");
			}
		} else {
			echo("Setting default IPv6 route through VPN.");
			exec("netsh interface ipv6 add route 2000::/3 \"" + tundevid +
			    "\" fe80::8 store=active");
		}
		echo("IPv6 route configuration done.");
	}

	if (env("CISCO_BANNER")) {
		echo("--------------------------------------------------");
		echo(env("CISCO_BANNER"));
		echo("--------------------------------------------------");
	}
	break;
case "disconnect":
	// Delete direct route for the VPN gateway
	exec("route delete " + env("VPNGATEWAY") + " mask 255.255.255.255");
}

log.Close();
WScript.Quit(0);
