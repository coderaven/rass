#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <stdio.h>

using namespace std;

// Coded By Raven G. Duran MUST BSIT-3R1 for Linux Server Project Automation (c) GPL OpenSource 2013

int main(){
	char domain[100],nameServer[100],subdomains[100][100],cname[100],directories[100][100],bindDirectory[100],apacheDirectory[100];
	int nSub, i, cnameChoice, choice, IP[4];

	printf("       ____      _    ____ ____        \n");
	printf(" _ _  |  _ \\    / \\  / ___/ ___|   _ _ \n");
	printf("(_|_) | |_) |  / _ \\ \\___ \\___ \\  (_|_)\n");
	printf(" _ _  |  _ <  / ___ \\ ___) |__) |  _ _ \n");
	printf("(_|_) |_| \\_\\/_/   \\_\\____/____/  (_|_)\n");
	printf("---   RAVEN AUTOMATED SERVER SETUP      ---\n");

	printf("\n(Be sure to run this program with the sudo command)\n\n");

	printf("\n::: INSTALLATION DIRECTORY :::\n");
	printf("Before I can automate your server configuration, I need to know where is the\n");
	printf("installation folder of BIND9 and APACHE2. \n(Do not forget the slash at the start and end of the directory)\n");
	printf("\nBIND9 Directory: (You can type the common default which is: /etc/bind/\n");
	printf("-> "); cin >> bindDirectory;
	printf("\nApache2 Directory: (You can type the common default which is: /etc/apache2/\n");
	printf("-> "); cin >> apacheDirectory;
	
	printf("\n::: BIND DNS SETUP :::\n");
	
	printf("\nEnter IP Address to use for your name server: (ex. 127.10.5.1)\n");
	printf("Note: Enter 'ifconfig' (without quotes) in another command line session first to \nsee what is your local server address for l0\n");
	printf("\nEx. if your l0 Ipv4 address is 127.0.0.1 then you can use an Ip address \nof the same class A Network like 127.7.8.1\n");
	printf("-> ");

	scanf("%d.%d.%d.%d",&IP[0],&IP[1],&IP[2],&IP[3]); // Scan IP Address

	printf("Enter the name of your domain: (Ex. mydomain.com)\n");
	printf("-> ");
	cin >> domain;
	printf("\nEnter the name of your Name Server: \n(Ex. server for server.mydomain.com or ns1 for ns1.mydomain.com \n.. no need to write domain name)\n");
	printf("-> ");
	cin >> nameServer;
	printf("\nDo you want to have a CNAME? (ex. www)\n");
	printf("(1)Yes or (Other Numbers)No \n-> ");
	cin >> cnameChoice;
	fflush(stdin);
	
	if (cnameChoice == 1){
		printf("\nWhat is the canonical name? \n(Ex. www or www1 or blah .. No need to put your domain)\n");
		printf("-> ");
		cin >> cname;
	}

	fflush(stdin);
	
	printf("\nEnter the number of subdomains: ");
	cin >> nSub;

	printf("(Input only subdomain not the whole address. Ex. blog not blog.mydomain.com)\n");

	for (i = 0; i < nSub; i++){
		printf("Enter subdomain %d: ",i+1);
		cin >> subdomains[i];
		fflush(stdin);
	}

	printf("\n::: APACHE WEB SERVER SETUP :::\n");
	printf("Do you want to manually choose where to store web files?\n");
	printf("(1) Yes, I want to manually choose \n(Other Numbers) No, I'd like you to automate it for me (PREFFERED) \n-> ");
	cin >> choice;
	
	if (choice == 1){
		printf("\nWhere do you want to store web files for %s? (ex. /var/www/%s/)\n",domain,domain);
		printf("(Do not forget the slash at the start and end of the directory)\n");
		printf("-> ");
		cin >> directories[0];
		for(i = 0; i < nSub; i++){
			printf("\nWhere do you want to store web files for %s.%s? (ex. /var/www/%s.%s/)\n",subdomains[i],domain,subdomains[i],domain);
			printf("-> ");
			cin >> directories[i+1];
		}
	} else {
		printf("\n:: Automatically choosing best directories for your website ... done!\n");
		sprintf(directories[0],"/var/www/%s/",domain);
		printf("%s webfiles is stored in %s\n",domain,directories[0]);
		for (i = 0; i < nSub; i++){
			sprintf(directories[i+1],"/var/www/%s.%s/",subdomains[i],domain);
			printf("%s.%s webfiles is stored in %s\n",subdomains[i],domain,directories[i+1]);
		}		
	}

	printf("\n\n");
	printf("\nFinalizing Everything ...\n");

	// Bind Configuration
	char filename[100]; // Filename for db.mydomain.com (Zone File)
	sprintf(filename,"db.%s",domain);
	printf("File name is: %s\n",filename);
	ofstream theFile (filename,ios::out); // Open file for Output

	// ----------------- Bind Zone File ------------ //
	stringstream buffer;
	buffer <<
		"; BIND data file for " << domain << endl <<
		";" << endl <<
		"$TTL	604800" << endl <<
		"@	IN	SOA	" << nameServer << "." << domain << ". root." << domain << ". (" << endl <<
		"			      2		; Serial"  << endl <<
		"			 604800		; Refresh" << endl <<
		"			  86400		; Retry"   << endl <<
		"			2419200		; Expire"  << endl <<
		"			 604800 )	; Negative Cache TTL" << endl << endl <<
		"@	IN	NS	" << nameServer << "." << domain << "." << endl;

	theFile << buffer.str() << nameServer << "	IN	A	" << IP[0] << "." << IP[1] << "." << IP[2] << "." << IP[3] << endl;
	
	// Adding Main Domain
	theFile << "@	IN	A	" << IP[0] << "." << IP[1] << "." << IP[2] << "." << IP[3]+1 << endl;
	// Adding CNAME
	if (cnameChoice == 1) theFile << "www	IN	CNAME	" << domain << "." << endl;
	// Adding Sub Domains
	for (i = 0; i < nSub; i++){
		theFile << subdomains[i] << "	IN	A	" << IP[0] << "." << IP[1] << "." << IP[2] << "." << IP[3]+2+i << endl;
	}	

	theFile.close();
	
	// ----------------- Bind Reverse File ------------ //
	sprintf(filename,"db.%s.reverse",domain);
	printf("Reverse Zone File name is: %s\n",filename);
	theFile.open(filename,ios::out); // Open file for Output

	theFile << buffer.str() << endl;
	
	// Adding Server and Domain Name Reverse
	theFile << IP[3] << "	IN	PTR	" << nameServer << "." << domain << "." << endl;
	theFile << IP[3]+1 << "	IN	PTR	" << domain << "." << endl;
	
	// Adding Pointers for sub domains
	for (i = 0; i < nSub; i++){
		theFile << IP[3]+2+i << "	IN	PTR	" << subdomains[i] << "." << domain << "." << endl;
	}

	theFile << "; This is the reverse of " << domain << " zone file" << endl;
	theFile.close();

	// ----------------- Bind Append to named.conf.local ------------ //
	buffer.str("");
	buffer << "cp " << bindDirectory << "named.conf.local ."; // Copy named.conf.local to current directory
	system(buffer.str().c_str());
	theFile.open("./named.conf.local",ios::app); // Open file to Append
	
	theFile << endl << endl <<
		"zone \"" << domain << "\" {" << endl <<
		"	type master;" << endl <<
		"	file \"" << bindDirectory << "db." << domain << "\";" << endl <<
		"};" << endl << endl <<
		"zone \"" << IP[2] << "." << IP[1] << "." << IP[0] << ".in-addr.arpa\" {" << endl <<
		"	type master;" << endl <<
		"	file \"" << bindDirectory << "db." << domain << ".reverse\";" << endl <<
		"};" << endl << endl;

	theFile.close();

	// Apache Configuration
	// For Primary Domain
	
	printf("... Setting up and automating configuration .....\n");

	theFile.open(domain,ios::out); // Create Website File For Apache
	buffer.str("");
	buffer <<
		"<VirtualHost *:80>" << endl <<
		"	ServerAdmin webmaster@" << domain << endl <<
		"	ServerName  " << domain << endl;
	if (cnameChoice == 1) buffer << "	ServerAlias  " << cname << "." << domain << endl;
	buffer <<
		"	DocumentRoot " << directories[0] << endl <<
		"	<Directory />" << endl <<
		"		Options FollowSymLinks" << endl <<
		"		AllowOverride None" << endl <<
		"	</Directory>" << endl <<
		"	<Directory " << directories[0] << ">" << endl <<
		"		Options Indexes FollowSymLinks MultiViews" << endl <<
		"		AllowOverride None" << endl <<
		"		Order allow,deny" << endl <<
		"		allow from all" << endl <<
		"	</Directory>" << endl <<
		"</VirtualHost>" << endl;
	theFile << buffer.str() << endl;
	theFile.close();

	// For SubDomains
	for (i = 0; i < nSub; i++){
		buffer.str("");
		buffer << subdomains[i] << "." << domain;
		theFile.open(buffer.str().c_str(),ios::out); // Create Website File For Apache
		buffer.str("");
		buffer <<
			"<VirtualHost *:80>" << endl <<
			"	ServerAdmin webmaster@" << subdomains[i] << "." << domain << endl <<
			"	ServerName  " << subdomains[i] << "." << domain << endl <<
			"	DocumentRoot " << directories[i+1] << endl <<
			"	<Directory />" << endl <<
			"		Options FollowSymLinks" << endl <<
			"		AllowOverride None" << endl <<
			"	</Directory>" << endl <<
			"	<Directory " << directories[i+1] << ">" << endl <<
			"		Options Indexes FollowSymLinks MultiViews" << endl <<
			"		AllowOverride None" << endl <<
			"		Order allow,deny" << endl <<
			"		allow from all" << endl <<
			"	</Directory>" << endl <<
			"</VirtualHost>" << endl;
		theFile << buffer.str() << endl;
		theFile.close();
	}
	
	printf("... Collecting and organizing files ...\n");
	// Create a config files folder for all server configurations
	buffer.str("");
	buffer << "mkdir -p " << domain << "_config_files/bind/ " << domain << "_config_files/apache/";
	system(buffer.str().c_str());

	// :: Move all to the config files folder ::
	// Moving bind named.conf.local
	buffer.str("");
	buffer << "mv named.conf.local " << domain << "_config_files/bind/";
	system(buffer.str().c_str());
	// Moving bind Zone and Reverse Zone files
	buffer.str("");
	buffer << "mv db." << domain << " " << domain << "_config_files/bind/";
	system(buffer.str().c_str());
	buffer.str("");
	buffer << "mv db." << domain << ".reverse " << domain << "_config_files/bind/";
	system(buffer.str().c_str());
	// Moving Apache Files
	buffer.str("");
	buffer << "mv " << domain << " " << domain << "_config_files/apache/";    // Move main domain
	system(buffer.str().c_str());

	for (i = 0; i < nSub; i++){ // Move subdomains
		buffer.str("");
		buffer << "mv " << subdomains[i] << "." << domain << " " << domain << "_config_files/apache/";    // Move main domain
		system(buffer.str().c_str());	
	}
	
	printf("... Pre-Setup done!...\n\n");

	// ---- Pre-Setup Overview --- //
	printf("\n\n---------- Pre-Setup Overview ----------\n\n");
	// List IP Address
	printf("NETWORK IP ADDRESS: %d.%d.%d.%d\n\n",IP[0],IP[1],IP[2],IP[3]);
	// List Name Server
	printf("NAME SERVER: %s.%s\n\n",nameServer,domain);
	// List Primary Domain Name
	printf("DOMAIN NAME: %s\n",domain);
	printf("IP ADDRESS: %d.%d.%d.%d\n",IP[0],IP[1],IP[2],IP[3]+1);
	printf("WEB FILES AT: %s\n\n",directories[0]);
	// List Bind Config Files
	printf("ZONE FILE NAME: db.%s\n",domain);
	printf("REVERSE ZONE FILE NAME: db.%s.reverse\n\n",domain);
	// List Canonical Name if exists
	if (cnameChoice == 1) printf("CANONICAL NAME: %s.%s\n\n",cname,domain);
	// List all subdomains
	for (i = 0; i < nSub; i++){
		printf("SUBDOMAIN #%d: %s.%s\n",i+1,subdomains[i],domain);
		printf("IP ADDRESS: %d.%d.%d.%d\n",IP[0],IP[1],IP[2],IP[3]+2+i);
		printf("WEB FILES AT: %s\n\n",directories[i+1]);
	}
	printf("-------------------------------------------------------------\n\n");

	printf("Continue with the new domain? (WEBSITE CONFIGURATION UPLOAD WILL COMMENCE)\n");
	printf("(1)Yes, I want this now! \n(Other Numbers) No, I'm just testing this or maybe I'll do it later\n-> ");
	cin >> choice;

	if (choice == 1){
	   printf("\n... Copying and uploading files ... \n");
	   // Upload Apache Files
	   buffer.str("");
	   buffer << "cp -r " << domain << "_config_files/apache/. " << apacheDirectory << "sites-available/"; 
	   system(buffer.str().c_str());
	   // Upload Bind Files
           buffer.str("");
	   buffer << "cp -r " << domain << "_config_files/bind/. " << bindDirectory;
	   system(buffer.str().c_str());

	   // Creating the Web Files Directories and default index.html
           for (i = 0; i < nSub+1; i++){
		buffer.str("");
		buffer << "mkdir -p " << directories[i];
		system(buffer.str().c_str());

		if (i > 0){
			buffer.str("");
			buffer << "echo This is the default HTML Page for " << 
				   subdomains[i-1] << "." << domain << " (Generated by RASS) > " << directories[i] << "index.html";
			system(buffer.str().c_str());
		} else {
			buffer.str("");
			buffer << "echo This is the default HTML Page for " << 
				   domain << " (Generated by RASS) > " << directories[i] << "index.html";
			system(buffer.str().c_str());
		}
	   }

	   // Activating Domains
	   printf("... Activating domains...\n");
	   // Activate Main Site
	   buffer.str("");
	   buffer << "a2ensite " << domain;
	   system(buffer.str().c_str());
	   // Activate Subdomains
	   for (i = 0; i < nSub; i++){
		buffer.str("");
	  	buffer << "a2ensite " << subdomains[i] << "." << domain;
		system(buffer.str().c_str());
	   }

	   // Reload and Restart Apache2 and Bind9
	   printf("\n... Reloading and Restarting Bind9 and Apache2 ...\n");
	   system("service bind9 restart");
	   system("service bind9 reload");
	   system("service bind9 restart");
	   system("service apache2 reload");
	   system("service apache2 restart");

	   printf("\nDONE! ENJOY YOUR NEW DOMAIN! :)\n");
	   printf("(Test by visiting http://%s in your browser)\n",domain);
	} else {
	   printf("How sad -_- .. Well, till next time!\n");
	}

	


	printf("\n\n--- PLUG: NO PENGUIN WAS HARM IN THE MAKING OF THIS COMMAND LINE TOOL --- \n");
	return 0;
}
