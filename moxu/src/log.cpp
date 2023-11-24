#include"log.h"
#include<ctype.h>
#include<map>
#include<functional>
#include<memory>
#include"config.h"

/*#--宏定义中，将后面的内容进行字符串化*/

namespace  moxu{

		const  char* LogLevel::ToString(LogLevel::Level level){
				switch (level)
				{
		#define XX(name) \
			case LogLevel::name: \
				return #name; \
				break;
				
				XX(DEBUG);
				XX(INFO);
				XX(WARN);
				XX(ERROR);
				XX(FATAL);

		#undef XX
				default:
					return "UNKNOW";
				}
				return "UNKNOW";
		}

		LogLevel::Level LogLevel::FromString(const std::string& str){
				if (str == "DEBUG") {
					return LogLevel::DEBUG;
				} else if (str == "INFO") {
					return LogLevel::INFO;
				} else if (str == "WARN") {
					return LogLevel::WARN;
				} else if (str == "ERROR") {
					return LogLevel::ERROR;
				} else if (str == "FATAL") {
					return LogLevel::FATAL;
				}
				return LogLevel::UNKNOW;
		}



	//LogEvent::LogEvent(std::shared_ptr<Logger> logger,LogLevel::Level level,const char* file,int32_t line,uint32_t elapse,uint32_t thread_id,uint32_t fiber_id,uint64_t time):
	LogEvent::LogEvent(std::shared_ptr<Logger> logger,LogLevel::Level level,const char* file,int32_t line,uint32_t elapse,uint32_t thread_id,uint32_t fiber_id,uint64_t time):
		m_file(file),
		m_elapse(elapse),
		m_fiberid(fiber_id),
		m_line(line),
		m_threadid(thread_id),
		m_time(time),
		m_logger(logger),
		m_level(level)
	{

	}


	void LogEvent::format(const char* fmt,...){
		va_list al;
		va_start(al,fmt);
		format(fmt,al);
		va_end(al);
	}
	void LogEvent::format(const char* fmt,va_list al){
		char* buf=nullptr;
		int len=vasprintf(&buf,fmt,al);//动态分配一个缓冲区buf来存储格式化后的字符串 
		//将要格式化的字符串与可变参数结合起来，将格式化和后的内容存储到buf中
		if(len!=-1){
			m_ss<<std::string(buf,len);
			free(buf);
		}
	}

	LogEventWrap::LogEventWrap(LogEvent::ptr e):m_event(e){
		//std::cout<<"封装Event的构造"<<std::endl;
	}

	LogEventWrap::~LogEventWrap(){
			m_event->getLogger()->log(m_event->getLevel(),m_event); //触发日志写入(logger->log())
	}

	std::stringstream& LogEventWrap::getSS(){
		return m_event->getSS();
	}


	LogEvent::~LogEvent(){
		
	}

		Logger::Logger(const std::string name):m_name(name),m_level(LogLevel::DEBUG){
				m_formatter.reset(new LogFormatter("%d%T%t%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));

				if(name  == "root"){
					 //就算我们没有任何的配置，他也户u会输出到控制台
					m_apperders.push_back(StdoutLogAppender::ptr(new StdoutLogAppender()));
				}

		}
		void Logger::log(LogLevel::Level level,LogEvent::ptr event){
				if(level>=m_level){
						auto self=shared_from_this(); //在自己的成员函数中获取自己的指针
						if(!m_apperders.empty()){
							for(auto & i: m_apperders){
									i->log(self,level,event); //
							}
						}else if(m_root){
							m_root->log(level,event);
						}

				}
		}

		void  Logger::debug(LogEvent::ptr event){

				log(LogLevel::DEBUG,event);
		}

		void Logger::info(LogEvent::ptr event){

				log(LogLevel::INFO,event);
		}
		void Logger::warn(LogEvent::ptr event){

				log(LogLevel::WARN,event);
		}
		void Logger::error(LogEvent::ptr event){

				log(LogLevel::ERROR,event);
		}
		void Logger::fatal(LogEvent::ptr event){

				log(LogLevel::FATAL,event);
		}


		void Logger::addAppender(LogAppender::ptr appender){
			if(!appender->getFormatter()){
				appender->setFormattwer(m_formatter); /*如果输出没有Formater的话 将我们自己的formater 保证每一个输出都有
				自己的Formater*/
			}
				m_apperders.push_back(appender);
		}
		void Logger::delAppender(LogAppender::ptr appender){
				for(std::list<LogAppender::ptr>::iterator it=m_apperders.begin();it!=m_apperders.end();++it){
						if(*it==appender){
								m_apperders.erase(it);
								break;
						}
				}
		}

		void Logger::clearAppenders(){
			m_apperders.clear();
		}


		void Logger::setFormatter(LogFormatter::ptr val){
			m_formatter=val;
		}
		
		void Logger::setFormatter(std::string & val){
			moxu::LogFormatter::ptr new_val(new moxu::LogFormatter(val));
			if(new_val->isError()){
				std::cout<<"Logger setFormatter name="<<m_name
						 <<" value="<<val<<" invalid formatter"<<std::endl;
				return;
			}
			m_formatter=new_val;
		}

		void Logger::setFormatter(std::string  val){
			moxu::LogFormatter::ptr new_val(new moxu::LogFormatter(val));
			if(new_val->isError()){
				std::cout<<"Logger setFormatter name="<<m_name
						 <<" value="<<val<<" invalid formatter"<<std::endl;
				return;
			}
			m_formatter=new_val;
		}

		LogFormatter::ptr Logger::getFormatter() const{
			return m_formatter;
		}


		FileLogAppender::FileLogAppender(const std::string filename):m_filename(filename){
			bool ans=reopen();
		}


		void  StdoutLogAppender::log(std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event){
			if(level>=m_level){
				std::string str=m_formatter->format(logger,level,event);
				std::cout<<str;//<<std::endl;
			}
	
		}
		void FileLogAppender::log(std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event){
			if(level>=m_level){
				m_filestream << m_formatter->format(logger,level,event)<<std::endl;  
			}
		}

		bool FileLogAppender::reopen(){
			if(m_filestream){ //如果是已经打开的
				m_filestream.close();
			}	
			m_filestream.open(m_filename);
			return !!m_filestream;
		}

		std::string LogFormatter::format(std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event){
				std::stringstream ss;
				//std::cout<<"此时的m_items的大小为"<<m_items.size()<<std::endl;
				for(auto & i : m_items){
					i->format(ss,logger,level,event);
				}
				return ss.str();
		}

		LogFormatter::LogFormatter(const std::string & pattern)
				:m_pattern(pattern) {
					init();
		}

		void LogFormatter::init(){ //解析日志输出
				//str format type
				std::vector<std::tuple<std::string,std::string,int>> vec; //tuple (string string int)
				std::string nstr;
				for(size_t i=0;i<m_pattern.size();++i){ //m_pattern是一个模式string
					if(m_pattern[i]!='%'){ 
						nstr.append(1,m_pattern[i]);//追加一个字
					    continue;
					}

					if((i+1)<m_pattern.size()){  //%%
						if(m_pattern[i+1]=='%'){
							nstr.append(1,'%');
						continue;
						}
					}

					size_t n=i+1;
					size_t fmt_begin=0;
					int format_status=0;//格式状态
					std::string fmt;
					std::string str;
					while(n<m_pattern.size()){
						if(!std::isalpha(m_pattern[n]) && m_pattern[n]!='{' &&
								m_pattern[n]!='}'){ //只接受字母
							str=m_pattern.substr(i+1,n-i-1);
							break; //日志格式出现非法字符 直接break
						}
						if(format_status == 0){ //尚未遇到{
							if(m_pattern[n] == '{'){
								str=m_pattern.substr(i+1,n-i-1);
								format_status = 1; //解析格式
								fmt_begin=n;
								++n;
								continue; 
							}		
						}

						else if (format_status==1)
						{
							if(m_pattern[n]== '}' ){
								fmt=m_pattern.substr(fmt_begin+1,n-fmt_begin-1);
								format_status=0;   // /   /==2代表{}字符匹配结束
								++n;
								break;
							}
						}
						++n;
						if(n==m_pattern.size()){
							if(str.empty()){
								str=m_pattern.substr(i+1);
							}
						}
					}

					if(format_status==0){
						if(!nstr.empty()){
							vec.push_back(std::make_tuple(nstr,std::string(),0));
							nstr.clear(); //删除nstr  将空间的首字符置为0
						}
						//str=m_pattern.substr(i+1,n-i-1);
						vec.push_back(std::make_tuple(str,fmt,1));
						i=n-1;
					}
					//格式错误
					else if(format_status==1){
						m_error=true;
						std::cout<<"pattern parse error "<<m_pattern<<" - "<<m_pattern.substr(i)<<std::endl;
						vec.push_back(std::make_tuple("<<pattern_error>>",fmt,1));
					}
					// else if(format_status==2){
					// 	if(!nstr.empty()){
					// 		vec.push_back(std::make_tuple(nstr,"",0));
					// 		nstr.clear();
					// 	}
					// 	vec.push_back(std::make_tuple(str,fmt,1));
					// 	i=n-1;
					// }

				}
				if(!nstr.empty()){
					vec.push_back(std::make_tuple(nstr,"",0));
				}

	
			
			const std::string m="m",p="p",c="c",r="r",t="t",n="n",d="d",l="l",f="f",T="T",F="F";
			static std::map<std::string,std::function<FormatItem::ptr (const std::string & str)> >
			 s_format_items = {
	#define XX(str,C) { \
		str,[](const std::string & fmt){ return FormatItem::ptr(new C(fmt)); } \
		}

				XX(f,FilenameFormatItem),
				XX(m,MessageFormatItem),
				XX(p,LevelFormatItem),
				XX(r,ElapseFormatItem),
				XX(c,FilenameFormatItem),
				XX(t,ThreadidFormatItem),
				XX(n,NewLineFormatItem),
				XX(d,DateTimeFormatItem),
				XX(l,LineFormatItem),
				XX(T,TabFormatItem),
				XX(F,FiberidFormatItem)
	#undef XX //取消定义宏
			};
			/*%m --消息体
			  %p --level
			  %r --启动后的时间
			  %c --日志名称
			  %t --线程id
			  %n --回车换行
			  %d --时间
			  %f --文件名
			  %l --行号*/

		for(auto & i : vec){
			if(std::get<2>(i)==0){ //string
				m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
			}
			else{
				auto it = s_format_items.find(std::get<0>(i));
				if(it == s_format_items.end()){
					m_items.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %"+
					std::get<0>(i)+">>")));
					m_error=true;
				}
				else{
					m_items.push_back(it->second(std::get<1>(i)));
				}
			}
			//std::cout<<"{"<<std::get<0>(i)<<"} - {"<<std::get<1>(i)<<"} - {"<<std::get<2>(i)<<"}"<<std::endl;
		}
		//std::cout<<"size="<<m_items.size()<<std::endl;
		
		}

	void LogFormatter::MessageFormatItem::format(std::ostream & os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event)
	{
			os<<event->getcontent();
	}//直接输出event里面

	void LogFormatter::LevelFormatItem::format(std::ostream & os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event){
			os<<LogLevel::ToString(level);
	}

	void LogFormatter::ElapseFormatItem::format(std::ostream & os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event){
			os<<event->getElapse();
	}

	void LogFormatter::NameFormatItem::format(std::ostream & os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event){
			os<<event->getLogger()->get_name();
	}

	void LogFormatter::ThreadidFormatItem::format(std::ostream & os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event){
			os<<event->getthread_id();
	}
	void LogFormatter::FiberidFormatItem::format(std::ostream & os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event){
			os<<event->getfiber_id();
	}

	void LogFormatter::DateTimeFormatItem::format(std::ostream & os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event){
			struct tm tm;
			time_t time=event->gettime();
			localtime_r(&time,&tm);
			char buf[1024];
			strftime(buf,sizeof buf,m_format.c_str(),&tm);

			os<<buf;
	}
	
	void LogFormatter::FilenameFormatItem::format(std::ostream & os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event){
			os<<event->getFile();
	}

	void LogFormatter::LineFormatItem::format(std::ostream & os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event){
			os<<" "<<event->getLine();
	}

	void LogFormatter::NewLineFormatItem::format(std::ostream & os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event){
			os<<std::endl;
	}


	void LogFormatter::StringFormatItem::format(std::ostream & os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event){
			os<<m_string;
	}


	void LogFormatter::TabFormatItem::format(std::ostream & os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event){
			os<<"\t";
	}

	struct LogAppenderDefine{
		int type = 0; //1 File, 2 StdOut
		LogLevel::Level m_level = LogLevel::Level::UNKNOW;
		std::string formatter;
		std::string file;

		bool operator==(const LogAppenderDefine& oth) const{
			return oth.file == file
				&& oth.formatter == formatter
				&& oth.m_level == m_level
				&& oth.type == type;
		}

		// bool operator<(const LogAppenderDefine& o) const{

		// }
	};

	struct LogDefine{
		std::string name;
		LogLevel::Level m_level = LogLevel::Level::UNKNOW;
		std::string formatter;

		std::vector<LogAppenderDefine> appenders;

		bool operator==(const LogDefine& oth)const { //接受的是常量对象，必须定义为常量成员函数，不然编译不通过
			return name == oth.name
				&& m_level == oth.m_level
				&& formatter == oth.formatter
				&& appenders == oth.appenders;
		}

		bool operator<(const LogDefine& o) const{
			return name < o.name;
		}

	};

//string->set<LogDefine> string转换成类
template<>
class LexicalCast<std::string,std::set<LogDefine> >{
public:
    std::set<LogDefine> operator()(const std::string& v){
            YAML::Node node = YAML::Load(v);  //string->结构
           	std::set<LogDefine> vec;
			for(size_t i =0;i<node.size();i++){
				const auto & n =node[i];
				if(!n["name"].IsDefined()){
					std::cout<<"log config error : name is null,"<<n<<std::endl;
					continue; //有问题就没必要往下解了
				}
				LogDefine ld;
				ld.name=n["name"].as<std::string>();
				ld.m_level=moxu::LogLevel::FromString(n["level"].IsDefined()?
							n["level"].as<std::string>() : "");
				if(n["formatter"].IsDefined()){
					ld.formatter = n["formatter"].as<std::string>();
				}
				if(n["appenders"].IsDefined()){
					for(size_t i = 0;i<n["appenders"].size();i++){
						auto a = n["appenders"][i];
						if(!a["type"].IsDefined()){
									std::cout<<"log config error : type is null,"<<a<<std::endl;
									continue; //有问题就没必要往下解了
						}
							std::string type  = a["type"].as<std::string>();
							LogAppenderDefine lad;
							if(type == "StdoutLogAppender"){
									lad.type=2;
							}	else if(type == "FileLogAppender"){
								lad.type=1;
								if(!n["file"].IsDefined()){
									std::cout<<"log config error :filelogappender  type is null,"<<a<<std::endl;
									continue; //有问题就没必要往下解了	
								
								}		
								lad.file=n["file"].as<std::string>();
								if(a["formatter"].IsDefined()){
									lad.formatter = a["formatter"].as<std::string>();
								}						
					}
					else{
								std::cout<<"log config error :apppender  type is invalid,"<<a<<std::endl;
									continue; //有问题就没必要往下解了
						}

						ld.appenders.push_back(lad);
				}
				}
				vec.insert(ld);
			}
			   std::cout<<"the "<<__func__<<" is running......"<<std::endl;
		return vec;
	}
};




//从set到string  类转换成string
template<>
class LexicalCast<std::set<LogDefine>,std::string>{
public:
    std::string operator()(const std::set<LogDefine>& v){
            YAML::Node node;
			for(auto&i:v){
				YAML::Node n;
				n["name"]=i.name;
				n["level"]=LogLevel::ToString(i.m_level);  //self
				if(i.formatter.empty()){
					n["level"]=i.formatter;
				}
				for(auto&a:i.appenders){
					YAML::Node na;
					if(a.type==1){
						na["type"]="FileLogAppender";
						na["file"]=a.file;
					}else if (a.type==2)
						na["type"]="StdoutLogAppender";
					na["level"]=LogLevel::ToString(a.m_level);

					if(a.formatter.empty()){
						na["formatter"] = a.formatter;
					}

					n["appenders"].push_back(na);
				}
				node.push_back(n);
			}
            // for(auto&i : v){
            //     //把他变成一个yaml节点 再把他放回去
            //     node.push_back(YAML::Load(LexicalCast<LogDefine,std::string>()(i)));
            // }
            std::stringstream ss;
            ss<<node;
            return ss.str();
       std::cout<<"the "<<__func__<<" is running......"<<std::endl;
	}
};

	struct LogIniter{

	 moxu::ConfigVar<std::set<LogDefine> >::ptr g_log_defines=
		moxu::Config::Lookup("logs",std::set<LogDefine>(),"logs config");


		LogIniter(){
			g_log_defines->addListener(100000000000,[](const std::set<LogDefine>& old_val,
					const std::set<LogDefine>& new_val) {
					std::cout<<"事件 is comming !"<<std::endl;
					//事件进来的时候，会触发这个函数
					 MOXU_LOG_INFO(MOXU_LOG_ROOT()) << "on_logger_conf_changed";
					//std::cout<<"on_logger_conf_changed"<<std::endl;
					//新增
					for(auto& i: new_val){
						Logger::ptr logger;	
						auto it  = old_val.find(i);
						if(it == old_val.end()){
							//是新增的 新的有 老的没有
							//logger.reset(new moxu::Logger(i.name));
							logger=MOXU_LOG_NAME(i.name);
						}else{
							//新的老的都有
							if(!(i == *it)){
								//修改的logger
								logger = MOXU_LOG_NAME(i.name);					
							}
						}
							logger->setlevel(i.m_level);
							if(!i.formatter.empty()){
							//if(typeid(i.formatter).name()=="std::string")
								logger->setFormatter(i.formatter);
							}

							logger->clearAppenders();
							for(auto & a : i.appenders){
								LogAppender::ptr ap;
								if(a.type == 1){ //File
									ap.reset(new FileLogAppender(a.file));
								}else if(a.type == 2) //Stdout
								{
									ap.reset(new StdoutLogAppender);
								}
								ap->setLevel(a.m_level);
								logger->addAppender(ap);
							}
					}
					//删除
					for(auto& i:old_val){
						auto it  = new_val.find(i);
						if(it == new_val.end())
						{
							//删除logger     不是真的删除
							Logger::ptr logger = MOXU_LOG_NAME(i.name);
							logger->setlevel((LogLevel::Level)100);
							logger->clearAppenders();
						}
					}
			});
			   std::cout<<"the "<<__func__<<" is running......"<<std::endl;
		}

	};

	static  LogIniter __log_init;

	void LoggerManager::init(){
			
	}

}