import {RadalMetricsProvider} from "@/presentation/components/metrics/metrics-provider";
import { BrowserRouter } from "react-router-dom";
import { AppProviders } from "@/app/providers";
import { AppRoutes } from "@/presentation/routes/app-routes";
import { Toaster } from "sonner";
import { StartupWelcome } from "@/presentation/components/ui/startup-welcome";
import { CheckoutModalProvider } from "@/presentation/components/checkout/checkout-modal-provider";
import { AnnouncementBanners } from "@/presentation/components/layout/announcement-banners";

export function App() {
  return (
    <StartupWelcome>
      <AppProviders>
        <BrowserRouter future={{ v7_startTransition: true, v7_relativeSplatPath: true }}>
          <CheckoutModalProvider><RadalMetricsProvider><AnnouncementBanners /><AppRoutes /></RadalMetricsProvider></CheckoutModalProvider>
          <Toaster
            position="bottom-right"
            richColors
            toastOptions={{
              classNames: {
                toast:
                  "!bg-white/80 !backdrop-blur-xl !border !border-slate-200/80 !shadow-lg !shadow-blue-900/10 !rounded-2xl !text-slate-800 !font-medium !tracking-tight dark:!bg-slate-900/80 dark:!border-slate-800",
                title: "!text-[0.95rem]",
                success: "!border-emerald-500/20 !bg-emerald-50/80 !text-emerald-900 dark:!bg-emerald-950/80 dark:!text-emerald-100",
                error: "!border-rose-500/20 !bg-rose-50/80 !text-rose-900 dark:!bg-rose-950/80 dark:!text-rose-100",
                info: "!border-sky-500/20 !bg-sky-50/80 !text-sky-900 dark:!bg-sky-950/80 dark:!text-sky-100",
              },
            }}
          />
        </BrowserRouter>
      </AppProviders>
    </StartupWelcome>
  );
}
