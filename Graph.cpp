#include "Graph.h"
#include<cstring>
#include <lemon/lgf_writer.h>

#define GRAPHVIZ "$PATHTUTOMIP/graphviz-2.40.1/bin/"

using namespace std;

#define epsilon 0.00001

//#define OUTPUT_GRAPH

/****************************  C_link  *******************************/

int C_link::return_other_extrem(int v){
	return (v==v1?v2:v1);
}

void C_link::set_algo_cost(double v){

  algo_cost=v;
}

/***************************  C_node  *****************************/

bool C_node::test_neighbour(int j){
  list<C_link*>::iterator it;
  for(it=L_adjLinks.begin() ; it !=L_adjLinks.end() ; it++){
    if((*it)->return_other_extrem(num) == j)
      return true;
  }
  return false;
}

bool C_node::test_successor(int j){
  list<C_link*>::iterator it;
  for(it=L_adjLinks.begin() ; it !=L_adjLinks.end() ; it++){
    if((*it)->return_other_extrem(num) == j)
      return true;
  }
  return false;
}

/**************************  C_Graph  ******************************/

void C_Graph::read_undirected_VRP(istream & fic){
  if (!fic){
    cout<<"File Error"<<endl;
  }else{
    int k,i,j;
		int nb_trucks, nb_nodes, Q;
    string m1, temp;
    list<C_link>::iterator it;
    C_link *a;
    //std::string::size_type sz;

    fic >> m1;
		
    // Jump the file description and go to the data
    while (m1 != "EOF"){
      if (m1 == "trucks:"){
      	fic >> temp;
				temp.erase(temp.size() - 1);
				nb_trucks = std::stoi(temp, nullptr, 10);
			}
			if (m1 == "DIMENSION"){
				fic >> m1;
				fic >> nb_nodes;
				V_nodes.resize(nb_nodes);
    		V_links.resize(nb_nodes * (nb_nodes - 1) / 2);
			}
			if (m1 == "CAPACITY"){
				fic >> m1;
				fic >> Q;
			}
			if (m1 == "NODE_COORD_SECTION"){
				for (i = 0 ; i < nb_nodes ; i++){
				  V_nodes[i].num = i;
				  V_nodes[i].L_adjLinks.clear();
				  fic >> m1;
				  fic >> V_nodes[i].x;
				  fic >> V_nodes[i].y;
    		}
			}
			if (m1 == "DEMAND_SECTION"){
				for (i = 0 ; i < nb_nodes ; i++){
				  fic >> m1;
				  fic >> V_nodes[i].weight;
    		}
			}
      fic >> m1;
    }
    std::cout << "nombre de camions : " << nb_trucks << "\n";
    std::cout << "nombre de noeuds : " << nb_nodes << "\n";
    std::cout << "capacite : " << Q << "\n";
    std::cout << "coord du noeud 3 : (" << V_nodes[2].x << "," << V_nodes[2].y << ")\n";

    directed=false;
    
    k = 0;
    for (i = 0 ; i < nb_nodes ; i++){
    	for (j = i + 1 ; j < nb_nodes ; j++){
    		a = new C_link;
    		a -> num = k;
      	a -> v1 = i;
      	a -> v2 = j;
      	a -> length = std::sqrt(std::pow(V_nodes[i].x - V_nodes[j].x, 2) + std::pow(V_nodes[i].y - V_nodes[j].y, 2));
      	V_nodes[i].L_adjLinks.push_back(a);
      	V_nodes[j].L_adjLinks.push_back(a);
     	  V_links[k] = a;
    		k++;
    		if ((i == 0) && (j == 2)){
    			std::cout << "ditance du noeud 3 au noeud 1 : " << a -> length << "\n";
    		}
    	}
    }
	
  }

  construct_Undirected_Lemon_Graph();

}


void C_Graph::read_undirected_DIMACS(istream & fic){
  if (!fic){
    cout<<"File Error"<<endl;
  }else{
    int k,i,j;
    string m1,m2;
    list<C_link>::iterator it;
    C_link *a;

    fic>>m1;
    fic>>m2;
		
    // Jump the file description and go to the data
    while (((m1!="p")&&(m2!="edge"))||((m1!="p")&&(m2!="col"))){
      m1=m2;
      fic>>m2;
    }

    directed=false;
    
    fic>>nb_nodes;
    fic>>nb_links;
		
    V_nodes.resize(nb_nodes);
    V_links.resize(nb_links);

    for (i=0;i<nb_nodes;i++){
      V_nodes[i].num = i;
      V_nodes[i].L_adjLinks.clear();
      V_nodes[i].weight=1;
    }

    for (k=0;k<nb_links;k++){
      fic>>m1;
      fic>>i;
      fic>>j;

      a=new C_link;
      a->num=k;
      a->v1=min(i-1,j-1);
      a->v2=max(i-1,j-1);
      a->length=0;
      V_nodes[i-1].L_adjLinks.push_back(a);
      V_nodes[j-1].L_adjLinks.push_back(a);
      V_links[k] = a;
    }
	
  }

  construct_Undirected_Lemon_Graph();

}




void C_Graph::read_directed_GRA(istream & fic){

   
 if (!(fic))
    cerr<<"Error occured of a gra format file"<<endl;
 else{

   int i,k;
   string sk;
   
   directed=true;
   
   fic>>nb_nodes;

   V_nodes.resize(nb_nodes);

   for (i=0;i<nb_nodes;i++){
     V_nodes[i].num=i;
     V_nodes[i].weight=1;
     V_nodes[i].L_adjLinks.clear();
   }

   nb_links=0;
   
   for (i=0;i<nb_nodes;i++){

     fic>>k;
     fic>>sk;
     
     fic>>k;
     while (k!=-1){
       C_link *a=new C_link;
       a->num=nb_links;
       a->v1=i;
       a->v2=k;
       a->length=0;       
       V_nodes[i].L_adjLinks.push_back(a);
       fic>>k;
       nb_links++;
     }

   }

   list<C_link*>::const_iterator it;
   V_links.resize(nb_links);
   k=0;
   for (i=0;i<nb_nodes;i++){
     for (it=V_nodes[i].L_adjLinks.begin();it!=V_nodes[i].L_adjLinks.end();it++){
       V_links[k]=*it;
       k++;
     }
  }

 }

 construct_Directed_Lemon_Graph();
 
}



void C_Graph::write_dot_G(string InstanceName){
  list<C_link>::iterator it;
  int i,k;

  ostringstream FileName; 
  FileName.str("");
  FileName <<InstanceName.c_str() << "_G.dot";

  ofstream fic(FileName.str().c_str());

  if (!directed) fic<<"graph G {"<<endl;
            else fic<<"digraph G {"<<endl;

    for(i=0 ; i<nb_nodes ; i++)
      fic<<"  "<<V_nodes[i].num<<"[shape = octagon]"<<endl;

    if (!directed)
      for(k=0 ; k<nb_links ; k++)
	fic<<"  \""<<V_links[k]->v1<<"\"--\""<<V_links[k]->v2<<"\";"<<endl;
    else
      for(k=0 ; k<nb_links ; k++)
	fic<<"  \""<<V_links[k]->v1<<"\"->\""<<V_links[k]->v2<<"\";"<<endl;
  
    fic<<"}"<<endl;

  

  fic.close();

  ostringstream commande; 
  commande.str("");
  commande<<GRAPHVIZ<<"dot -Tpdf -o "<<InstanceName.c_str() << "_G.pdf "<< FileName.str().c_str()<<endl;
  cout<<commande.str().c_str();
  if(system(commande.str().c_str())){cout<<"PDF generated successfully"<<endl;}
  return;
}






void C_Graph::write_dot_G_stableset(string InstanceName, vector<int>& stable){
  int i,k;
  ostringstream FileName; 
  FileName.str("");
  FileName <<InstanceName.c_str() << "_G_stable.dot";

  ofstream fic(FileName.str().c_str());
  
  fic<<"graph G {"<<endl;
  
  for(i=0 ; i<nb_nodes ; i++){
    if (stable[i]>1-epsilon)
      fic<<"  "<<V_nodes[i].num<<"[shape = octagon]"<<endl;
    else
      if (stable[i]<epsilon)
	fic<<"  "<<V_nodes[i].num<<"[shape = octagon, color=white]"<<endl;
  }
  
  for(k=0 ; k<nb_links ; k++){
    if ((stable[V_links[k]->v1]>1-epsilon)&&(stable[V_links[k]->v2]>1-epsilon))
      fic<<"  \""<<V_links[k]->v1<<"\"--\""<<V_links[k]->v2<<"\";"<<endl;
    else
      fic<<"  \""<<V_links[k]->v1<<"\"--\""<<V_links[k]->v2<<"\" [color=white];"<<endl;
  }
  
  fic<<"}"<<endl;
		
  fic.close();

  ostringstream commande; 
  commande.str("");
  commande<<GRAPHVIZ<<"dot -Tpdf -o "<<InstanceName.c_str() << "_G_stable.pdf "<< FileName.str().c_str()<<endl;
  cout<<commande.str().c_str();
  if (system(commande.str().c_str())){cout<<"PDF generated successfully"<<endl;}


}





void C_Graph::write_dot_G_color(string InstanceName, vector<int>& coloring){
  int i,k;
  ostringstream FileName; 
  FileName.str("");
  FileName <<InstanceName.c_str() << "_G_color.dot";

  vector <string> colors;
  colors.push_back("green");
  colors.push_back("blue");
  colors.push_back("red");
  colors.push_back("cyan");
  colors.push_back("yellow");
  colors.push_back("magenta");
  colors.push_back("darkorchid");
  colors.push_back("darkorange");
  colors.push_back("deeppink");
  colors.push_back("forestgreen3");
  colors.push_back("indigo");
  colors.push_back("midnightblue");
  colors.push_back("violetred");

  int chi=0;
  for (i=0;i<nb_nodes;i++)
    if (chi<coloring[i]) chi=coloring[i];

  
  if(chi >= (int)colors.size()){
    cout<<"We only have 13 colors and this solutions needs "<<chi<<" colors... some nodes will have wrong colors!"<<endl;
  }
  
  ofstream fic(FileName.str().c_str());
  fic<<"graph G {"<<endl;
  
  for(i=0 ; i<nb_nodes ; i++){
    fic<<"  "<<V_nodes[i].num<<"[shape = octagon, style = filled , fillcolor = "<<colors[(coloring[V_nodes[i].num]) % colors.size()]<<" ]"<<endl;
  }
  
  for(k=0 ; k<nb_links ; k++){
      fic<<"  \""<<V_links[k]->v1<<"\"--\""<<V_links[k]->v2<<"\";"<<endl;
  }
  
  fic<<"}"<<endl;
		
  fic.close();

  ostringstream commande; 
  commande.str("");
  commande<<GRAPHVIZ<<"dot -Tpdf -o "<<InstanceName.c_str() << "_G_color.pdf "<< FileName.str().c_str()<<endl;
  cout<<commande.str().c_str();
  if (system(commande.str().c_str())){cout<<"PDF generated successfully"<<endl;}
  

}



void C_Graph::write_dot_directed_G_induced(string InstanceName, vector<int>& sol){
  int i,k;
  ostringstream FileName; 
  FileName.str("");
  FileName <<InstanceName.c_str() << "_G_stable.dot";

  ofstream fic(FileName.str().c_str());
  
  fic<<"digraph G {"<<endl;
  
  for(i=0 ; i<nb_nodes ; i++){
    if (sol[i]>1-epsilon)
      fic<<"  "<<V_nodes[i].num<<"[shape = octagon]"<<endl;
    else
      if (sol[i]<epsilon)
	fic<<"  "<<V_nodes[i].num<<"[shape = octagon, color=white]"<<endl;
  }
  
  for(k=0 ; k<nb_links ; k++){
    if ((sol[V_links[k]->v1]>1-epsilon)&&(sol[V_links[k]->v2]>1-epsilon))
      fic<<"  \""<<V_links[k]->v1<<"\"->\""<<V_links[k]->v2<<"\";"<<endl;
    else
      fic<<"  \""<<V_links[k]->v1<<"\"->\""<<V_links[k]->v2<<"\" [color=white];"<<endl;
  }
  
  fic<<"}"<<endl;
		
  fic.close();

  ostringstream commande; 
  commande.str("");
  commande<<GRAPHVIZ<<"dot -Tpdf -o "<<InstanceName.c_str() << "_G_acyclic.pdf "<< FileName.str().c_str()<<endl;
  cout<<commande.str().c_str();
  if (system(commande.str().c_str())){cout<<"PDF generated successfully"<<endl;}


}


void C_Graph::read_undirected_complete_TSP(istream & fic){
  int i,j,k;
  char ch[100];  
  C_link *a;
  
 fic>>ch;
 while ((strcmp(ch,"DIMENSION"))){
   fic>>ch;
 }

 fic>>ch;
 if ((strcmp(ch,":"))) {
   nb_nodes=atoi(ch);
 }
 else{
   fic>>nb_nodes;
 }
 
 cout<<nb_nodes<<endl;
 
 V_nodes.resize(nb_nodes);

 fic>>ch;
 while ((strcmp(ch,"NODE_COORD_SECTION")))
   fic>>ch;

 maxx=0,maxy=0,minx=1e6,miny=1e6;
 for (i=0;i<nb_nodes;i++){
   fic>>ch;
   fic>>V_nodes[i].x;
   fic>>V_nodes[i].y;
   if (maxx<V_nodes[i].x) maxx= V_nodes[i].x;
   if (maxy<V_nodes[i].y) maxy= V_nodes[i].y;
   if (minx>V_nodes[i].x) minx= V_nodes[i].x;
   if (miny>V_nodes[i].y) miny= V_nodes[i].y;
 }

 V_links.resize((nb_nodes*(nb_nodes-1))/2);
 k=0;
 for (i=0;i<nb_nodes;i++)
   for (j=i+1;j<nb_nodes;j++){
     a=new C_link;
     a->num=k;
     a->v1=i;
     a->v2=j;
     a->length=lengthTSP(i,j);
     V_nodes[i].L_adjLinks.push_back(a);
     V_nodes[j].L_adjLinks.push_back(a);
     V_links[k] = a;
     k++;
   }
 
 directed=false;
 construct_Undirected_Lemon_Graph();
 
}

float C_Graph::lengthTSP(int i, int j){
return sqrt(pow(V_nodes[i].x-V_nodes[j].x,2)+pow(V_nodes[i].y-V_nodes[j].y,2));
}


void C_Graph::write_SVG_node_cloud(string InstanceName){
  int i, color;
  ostringstream FileName; 
  FileName.str("");
  FileName <<InstanceName.c_str() << "_G_nodes.svg";


  color= 0;
  float dimx=500;
  float dimy=500;
  
  ofstream fic(FileName.str().c_str());

  fic<<"<html><body><svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.2\"";
  fic<<" width=\"100%\" height=\"100%\"";
  fic<<" viewBox=\""<<-2.0<<" "<<-2.0<<" "<<dimx+7.0<<" "<<dimy+7.0<<"\"";
  fic<<" preserveAspectRatio=\"yes\">"<<endl;
  fic<<"<g>"<<endl<<endl;


  for (i=0;i<nb_nodes;i++){

        fic<<"<circle cx=\""<<dimx*(V_nodes[i].x-minx)/(maxx-minx)<<"\" cy=\""<<dimy*(V_nodes[i].y-miny)/(maxy-miny)<<"\" r=\"2\" stroke=\""<<color<<"\" stroke-width=\"1\" fill=\""<<color<<"\" />"<<endl;

  }

  fic<<endl<<endl<<"</g></svg></body></html>"<<endl;
  fic.close();

}

void C_Graph::write_SVG_tour(string InstanceName, list<pair<int,int> >& sol){

  int i;
  ostringstream FileName; 
  FileName.str("");
  FileName <<InstanceName.c_str() << "_G_tour.svg";

  float dimx=500;
  float dimy=500;
  
  ofstream fic(FileName.str().c_str());

  fic<<"<html><body><svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.2\"";
  fic<<" width=\"100%\" height=\"100%\"";
  fic<<" viewBox=\""<<-2.0<<" "<<-2.0<<" "<<dimx+7.0<<" "<<dimy+7.0<<"\"";
  fic<<" preserveAspectRatio=\"yes\">"<<endl;
  fic<<"<g>"<<endl<<endl;


  for (i=0;i<nb_nodes;i++){

        fic<<"<circle cx=\""<<dimx*(V_nodes[i].x-minx)/(maxx-minx)<<"\" cy=\""<<dimy*(V_nodes[i].y-miny)/(maxy-miny)<<"\" r=\"2\" stroke=\"0\" stroke-width=\"1\" fill=\"0\" />"<<endl;

  }

  list<pair<int,int> >::const_iterator it;
  for (it=sol.begin();it!=sol.end();it++){
    fic<<"<line x1=\""<<dimx*(V_nodes[it->first].x-minx)/(maxx-minx)<<"\"";
    fic<<" y1=\""<<dimy*(V_nodes[it->first].y-miny)/(maxy-miny)<<"\"";
    fic<<" x2=\""<<dimx*(V_nodes[it->second].x-minx)/(maxx-minx)<<"\"";
    fic<<" y2=\""<<dimy*(V_nodes[it->second].y-miny)/(maxy-miny)<<"\"";
    fic<<" style=\"stroke:rgb(255,0,0);stroke-width:2\" />"<<endl;
  }

  fic<<endl<<endl<<"</g></svg></body></html>"<<endl;
  fic.close();

}


//////////////////////////////////////////////////
/////////////////////////////////////////////////

bool C_Graph::detect_circuit(vector<int>&sol){
  list<C_link*>::const_iterator it;
  int i,cpt,totnode;
  vector<int> v_label;
  bool cycle;

  list<pair<int,list<C_link*>::const_iterator> > P;
  P.clear();

  v_label.resize(nb_nodes);

  totnode=0;
  for (i=0;i<nb_nodes;i++){
    v_label[i]=-1;  // node outside the solution
    if (sol[i]==1){
       v_label[i]=0; // no-visited node
       totnode++;
    }
  }

  cycle=false;
  cpt=0;

  while ((!cycle)&&(cpt!=totnode)){

    i=0;
    while (v_label[i]!=0)
      i++;
  
    P.push_back(make_pair(i,V_nodes[i].L_adjLinks.begin()));
    v_label[i]=1;  // visited node
    cpt++;

    while ((!cycle)&&(!(P.empty()))){

          if (P.back().second==V_nodes[P.back().first].L_adjLinks.end()){
              v_label[P.back().first]=2;  // exploration done for this node
	      P.pop_back();
	  }
	  else{

	    it=P.back().second;
	    (P.back().second)++;
   
	    if (v_label[(*it)->v2]==0){
	      P.push_back(make_pair((*it)->v2,V_nodes[(*it)->v2].L_adjLinks.begin()));
	      v_label[(*it)->v2]=1;     // visited node
	      cpt++;
	    }
	    else{
	      if (v_label[(*it)->v2]==1){
		cycle=true;
	      }
	    }

	  }
    }
  }
  return cycle;
}

/////////////////////////////////////////////////

bool C_Graph::return_circuit(vector<int>&sol, list<int> &L){
  list<C_link*>::const_iterator it;
  int i,cpt,totnode;
  vector<int> v_label;
  bool cycle;
  int extr;
  L.empty();
  vector<int> T;

  list<pair<int,list<C_link*>::const_iterator> > P;
  P.clear();

  v_label.resize(nb_nodes);
  T.resize(nb_nodes);
  
  totnode=0;
  for (i=0;i<nb_nodes;i++){
    v_label[i]=-1;  // node outside the solution
    if (sol[i]==1){
       v_label[i]=0; // no-visited node
       totnode++;
    }
    T[i]=-2;
  }

  cycle=false;
  cpt=0;

  while ((!cycle)&&(cpt!=totnode)){

    i=0;
    while (v_label[i]!=0)
      i++;
  
    P.push_back(make_pair(i,V_nodes[i].L_adjLinks.begin()));
    v_label[i]=1;  // visited node
    T[i]=-1;
    cpt++;

    while ((!cycle)&&(!(P.empty()))){

          if (P.back().second==V_nodes[P.back().first].L_adjLinks.end()){
              v_label[P.back().first]=2;  // exploration done for this node
	      P.pop_back();
	  }
	  else{

	    it=P.back().second;
	    (P.back().second)++;
   
	    if (v_label[(*it)->v2]==0){
	      P.push_back(make_pair((*it)->v2,V_nodes[(*it)->v2].L_adjLinks.begin()));
	      v_label[(*it)->v2]=1;     // visited node
	      T[(*it)->v2]=(*it)->v1;
	      cpt++;
	    }
	    else{
	      if (v_label[(*it)->v2]==1){
		cycle=true;
		extr=(*it)->v2;
		T[(*it)->v2]=(*it)->v1;
	      }
	    }

	  }
    }
  }

  if (cycle){
    i=extr;
    L.push_back(i);
    i=T[i];
    while (i!=extr){
      L.push_back(i);
      i=T[i];
    }      
  }
 
  return cycle;
}


/******************* LEMON ******************/

void C_Graph::construct_Undirected_Lemon_Graph(){

  int i;
  list<C_link *>::const_iterator it;
 
  for (i=0;i<nb_nodes;i++){
    V_nodes[i].LGU_name=L_GU.addNode();
    L_rtnmap[L_GU.id(V_nodes[i].LGU_name)]=i;
  }
  for (i=0;i<nb_nodes;i++){
    for (it=V_nodes[i].L_adjLinks.begin();it!=V_nodes[i].L_adjLinks.end();it++){
	(*it)->LGU_name=L_GU.addEdge(V_nodes[i].LGU_name,V_nodes[(*it)->return_other_extrem(i)].LGU_name);

    }
  }

}




void C_Graph::construct_Directed_Lemon_Graph(){

  int i;
  list<C_link *>::const_iterator it;
 
  for (i=0;i<nb_nodes;i++){
    V_nodes[i].LGD_name=L_GD.addNode();
    L_rtnmap[L_GD.id(V_nodes[i].LGD_name)]=i;
  }
  for (i=0;i<nb_nodes;i++){
      for (it=V_nodes[i].L_adjLinks.begin();it!=V_nodes[i].L_adjLinks.end();it++){
	(*it)->LGD_name=L_GD.addArc(V_nodes[i].LGD_name,V_nodes[(*it)->return_other_extrem(i)].LGD_name);
      }
  }
  #ifdef OUTPUT_GRAPH
  lemon::DigraphWriter<lemon::ListDigraph> WW(L_GD, cout);
  WW.run();
  #endif
}



#define PREC 1000

double C_Graph::Undirected_MinimumCut(list<int>& W){
  int i;
  list<C_link *>::const_iterator it;
  lemon::ListGraph::EdgeMap<int> L_cost_int(L_GU);
  lemon::ListGraph::NodeMap<bool> mincut(L_GU);
  double mincutvalue;
  
  for (i=0;i<nb_nodes;i++){
    for (it=V_nodes[i].L_adjLinks.begin();it!=V_nodes[i].L_adjLinks.end();it++){
      if (i<(*it)->return_other_extrem(i)) {
	//cout<<"("<<L_GU.id((*it)->LGU_name)<<","<<(*it)->algo_cost<<") ";
	L_cost_int.set((*it)->LGU_name,(*it)->algo_cost*PREC);
      }
    }
    }
  
  //cout<<endl;
  
  // cout<<"Algo Cost non-null: ";
  // for (i=0;i<nb_nodes;i++){
  //   for (it=V_nodes[i].L_adjLinks.begin();it!=V_nodes[i].L_adjLinks.end();it++){
  //     if (L_cost[(*it)->LGU_name]>epsilon)
  // 	cout<<"("<<i<<","<<(*it)->return_other_extrem(i)<<","<<L_cost[(*it)->LGU_name]<<") ";
  //   }
  // }
  // cout<<endl;
  
      
 
  lemon::NagamochiIbaraki<lemon::ListGraph, lemon::ListGraph::EdgeMap<int> > L_NI(L_GU,L_cost_int);
  

  L_NI.run();

  mincutvalue=L_NI.minCutMap (mincut)/(PREC*1.0);

  W.clear();
  for (i=0;i<nb_nodes;i++)
    if (mincut[V_nodes[i].LGU_name]) W.push_back(i);
  
  #ifdef OUTPUT_GRAPH  
  cout<<"MinCut value : "<<mincutvalue<<endl;
  cout<<"MinCut induced by : ";
  for (i=0;i<nb_nodes;i++)
    if (mincut[V_nodes[i].LGU_name]) cout<<i<<" ";
  cout<<endl;
  #endif

  return mincutvalue;

}



void C_Graph::Directed_ShortestPathTree(int u, vector<int>& T, vector<float>&dist){  
  int i;
  list<C_link *>::const_iterator it;
  lemon::ListDigraph::ArcMap<double> L_cost(L_GD);
  
  for (i=0;i<nb_nodes;i++){
    for (it=V_nodes[i].L_adjLinks.begin();it!=V_nodes[i].L_adjLinks.end();it++){
      L_cost.set((*it)->LGD_name,(*it)->algo_cost);
    }
  }
 
 lemon::Dijkstra<lemon::ListDigraph,lemon::ListDigraph::ArcMap<double> > L_Dij(L_GD,L_cost);

 L_Dij.run(V_nodes[u].LGD_name);

 for (i=0;i<nb_nodes;i++)
   if (i==u) {
     T[i]=-1;
     dist[i]=0;
   }
   else
     if (L_Dij.predNode(V_nodes[i].LGD_name)==lemon::INVALID) {
       T[i]=-2;
       dist[i]=-1;
     }
     else{
       T[i]=L_rtnmap[L_GD.id(L_Dij.predNode(V_nodes[i].LGD_name))];
       dist[i]=L_Dij.dist(V_nodes[i].LGD_name);
     }
 
}




double C_Graph::Directed_ShortestPath(int s, int t, list<int>& P){  
  int i,j;
  list<C_link *>::const_iterator it;
  lemon::ListDigraph::ArcMap<double> L_cost(L_GD);
  double val=0;
  
  for (i=0;i<nb_nodes;i++){
    for (it=V_nodes[i].L_adjLinks.begin();it!=V_nodes[i].L_adjLinks.end();it++){
      L_cost.set((*it)->LGD_name,(*it)->algo_cost);
    }
  }

 lemon::Dijkstra<lemon::ListDigraph,lemon::ListDigraph::ArcMap<double> > L_Dij(L_GD,L_cost);

 L_Dij.run(V_nodes[s].LGD_name, V_nodes[t].LGD_name);

 P.clear();
 
 if (L_Dij.predNode(V_nodes[t].LGD_name)!=lemon::INVALID) {

   P.push_front(t);
   i=t;
   while (L_Dij.predNode(V_nodes[i].LGD_name)!=lemon::INVALID){
     j=L_rtnmap[L_GD.id(L_Dij.predNode(V_nodes[i].LGD_name))];
     P.push_front(j);
     i=j;
   }     
 }
 
 #ifdef OUTPUT_GRAPH
 if (L_Dij.predNode(V_nodes[t].LGD_name)==lemon::INVALID)
   cout<<"No shortest path found between "<<s<<" and "<<t<<endl;
 else {
   list<int>::const_iterator iti;
   cout<<"Shortest Path value : "<<val<<endl;
   cout<<"Shortest Path from "<<s<<" to "<<t<<" : ";
   for (iti=P.begin();iti!=P.end();iti++)
     cout<<*iti<<" ";
   cout<<endl;
 }
 #endif
  
 return val;
}
