#ifndef _SINGLETON_H_
#define _SINGLETON_H_
template <class SingletonClass >
class Singleton{
public:
	static SingletonClass * instance(){
		static SingletonClass* instance = 0;
		if( !instance ){
			instance = new SingletonClass;
		}
		return instance;
	}
	SingletonClass* operator ->() { return instance(); }
	const SingletonClass* operator ->() const { return instance(); }
private:
	Singleton(){ }
	~Singleton(){ }
};
#endif // _SINGLETON_H_
