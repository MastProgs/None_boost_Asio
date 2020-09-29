//
// App.xaml.h
// App 클래스의 선언입니다.
//

#pragma once

#include "App.g.h"

namespace UWP_cpp
{
	/// <summary>
	/// 기본 애플리케이션 클래스를 보완하는 애플리케이션별 동작을 제공합니다.
	/// </summary>
	ref class App sealed
	{
	protected:
		virtual void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ e) override;

	internal:
		App();

	private:
		void OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ e);
		void OnNavigationFailed(Platform::Object ^sender, Windows::UI::Xaml::Navigation::NavigationFailedEventArgs ^e);
	};
}
